#pragma once
#ifndef __CG_BROAD_PHASE_H__
#define __CG_BROAD_PHASE_H__

#include <GPED/DynamicAABBTree.h>
#include <GPED/GPED_collide_fine.h>
#include <algorithm>

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
}

#endif