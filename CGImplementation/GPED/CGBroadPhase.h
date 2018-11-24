#pragma once
#ifndef __CG_BROAD_PHASE_H__
#define __CG_BROAD_PHASE_H__

#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GPED/DynamicAABBTree.h>
#include <GPED/GPED_collide_fine.h>
#include <Graphics/Shader.h>

namespace CGProj
{
	struct cgPair
	{
		int proxyIdA;
		int proxyIdB;
	};

	class CGBroadPhase
	{
	public:
		enum
		{
			e_nullProxy = -1
		};

		CGBroadPhase();
		~CGBroadPhase();

		int CreateProxy(const GPED::c3AABB& aabb, void* userData);
		void DestroyProxy(int proxyId);
		void UpdateProxy(int proxyId, const GPED::c3AABB& aabb, const glm::vec3& displacement);
		void UpdateProxy(int proxyId, const GPED::c3AABB& aabb);
		
		void* GetUserData(int proxyId) const;
		bool TestOverlap(int proxyIdA, int proxyIdB);

		int GetProxyCount() const;

		template<typename T>
		void UpdatePairs(T* callback);

		template<typename T>
		void Query(T* callback, const GPED::c3AABB& aabb) const;

		template<typename T>
		void RayCast(T* callback, const GPED::c3RayInput& input) const;

		const DynamicAABBTree* getTree() const
		{
			return &m_tree;
		}
	private:
		friend class DynamicAABBTree;

		void BufferMove(int proxyId);
		void UnBufferMove(int proxyId);

		bool QueryCallback(int proxyId);

		DynamicAABBTree m_tree;

		int m_proxyCount;

		int* m_moveBuffer;
		int m_moveCapacity;
		int m_moveCount;

		cgPair* m_pairBuffer;
		int m_pairCapacity;
		int m_pairCount;

		int m_queryProxyId;
	};

	inline bool cgPairLessThan(const cgPair& pair1, const cgPair& pair2)
	{
		if (pair1.proxyIdA < pair2.proxyIdA)
			return true;

		if (pair1.proxyIdA == pair2.proxyIdA)
			return pair1.proxyIdB < pair2.proxyIdB;

		return false;
	}

	template<typename T>
	inline void CGBroadPhase::UpdatePairs(T * callback)
	{
		// Rest pair buffer
		m_pairCount = 0;

		// Perform tree queries for all moving proxies.
		for (int i = 0; i < m_moveCount; ++i)
		{
			m_queryProxyId = m_moveBuffer[i];
			if (m_queryProxyId == e_nullProxy) continue;

			// We have to query the tree with the fat AABB so that
			// we don't fail to create a pair that may touch later.
			const GPED::c3AABB& fatAABB = m_tree.GetFatAABB(m_queryProxyId);

			// Query tree, create pairs and add them pair buffer.
			m_tree.Query(this, fatAABB);
		}

		// Reset move Buffer
		m_moveCount = 0;
		
		// Sort the pair buffer to expose duplicates.
		std::sort(m_pairBuffer, m_pairBuffer + m_pairCount, cgPairLessThan);

		// Send the pairs back to the client
		int i = 0;
		while (i < m_pairCount)
		{
			cgPair* primaryPair = m_pairBuffer + i;
			void* userDataA = m_tree.GetUserData(primaryPair->proxyIdA);
			void* userDataB = m_tree.GetUserData(primaryPair->proxyIdB);

			callback->AddPair(userDataA, userDataB);
			++i;

			// Skip any duplicate pairs
			while (i < m_pairCount)
			{
				cgPair* pair = m_pairBuffer + i;
				if (pair->proxyIdA != primaryPair->proxyIdA || pair->proxyIdB != primaryPair->proxyIdB)
				{
					break;
				}
				++i;
			}
		}
	}

	template<typename T>
	inline void CGBroadPhase::Query(T * callback, const GPED::c3AABB & aabb) const
	{
		m_tree.Query(callback, aabb);
	}

	template<typename T>
	inline void CGBroadPhase::RayCast(T * callback, const GPED::c3RayInput & input) const
	{
		m_tree.RayCast(callback, input);
	}

	/* 181116 Chanhaneg Lee
	   Wrapper Class to get pairs of potential collisions from the BroadPhase
	   You should pass this class to the parameter of the UpdatePairs method of BroadPhase
	   ex) BroadPhaseInstance.UpdatePairs(&BroadResultWrapper_Instance)

	   The vPairs member is using STL vector. So, It will be better to change the data structure
	   to manage memory alloc/free by our own intention. This work will be achieved later.
	*/
	template<class T>
	struct BroadResultWrapper
	{
		// You can pass the type you want for your purpose, and then
		// get the pairs matched with the type you passed.
		// For example, In physics engine, we will use the CollisionPrimitive Class.
		// In Graphics Engine, we will other class.
		typedef typename T* TypePointer;
	public:
		std::vector<std::pair<TypePointer, TypePointer>> vPairs;
		
		// Callback function of BroadPhase Result
		void AddPair(void* proxyUserDataA, void* proxyUserDataB)
		{
			vPairs.push_back
			(
				std::make_pair
				(
					(TypePointer)proxyUserDataA,
					(TypePointer)proxyUserDataB
				)
			);
		}

		BroadResultWrapper() 
		{ 
			#define BROAD_RESULT_WRAPPER_INIT_SIZE 200

			// Manual Setting of Memory Space of STL vector
			// The memory will be allocated dynamically if the number of elements will be
			// more than the macro.
			vPairs.reserve(BROAD_RESULT_WRAPPER_INIT_SIZE); 

			#undef BROAD_RESULT_WRAPPER_INIT_SIZE
		}
		~BroadResultWrapper() { vPairs.clear(); }
	}; // Struct BroadResultWrapper


	// BroadPhase AABB Renderer for Debug
	class BroadRendererWrapper
	{
	public:
		void draw(Shader* shader, glm::mat4* proj, glm::mat4* view);

		void connectTree(const DynamicAABBTree* tree) { m_tree = tree; }
		void setColor(const glm::vec3& leaf, const glm::vec3& branch)
		{
			leafColor = leaf;
			branchColor = branch;
		}
		void setLineWidth(float leaf, float branch)
		{
			leafWidth = leaf;
			branchWidth = branch;
		}
	private:
		const DynamicAABBTree* m_tree;
		Shader* m_shader;
		glm::mat4* m_projection;
		glm::mat4* m_view;

		glm::vec3 leafColor;
		glm::vec3 branchColor;
		float leafWidth = 1.f;
		float branchWidth = 1.f;

		void recursiveDraw(int index);
		void render(const GPED::c3AABB& aabb, const glm::vec3& Color, float lineWidth);
	};

	struct CGRayCastCallback
	{
		virtual bool process
		(
			const GPED::c3RayOutput& output,
			const GPED::c3RayInput& input,
			void* userData
		) = 0; // interface
	};

	struct BroadRayCastWrapper
	{
	public:
		bool RayCastCallback(const GPED::c3RayInput& input, int nodeId)
		{
			void* userData = broadPhase->GetUserData(nodeId);

			GPED::c3RayOutput output;
			bool hit = GPED::CollisionDetector::RayCollision(
				output, input, (GPED::CollisionPrimitive*)userData);

			if (hit)
			{
				return callback->process(output, input, userData);
			}

			return true;
		}

		const CGBroadPhase* broadPhase;
		CGRayCastCallback* callback;
	};
}

#endif