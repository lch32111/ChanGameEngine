#pragma once
#ifndef __DYNAMIC_AABB_TREE_H__
#define __DYNAMIC_AABB_TREE_H__

#include <GPED/GPED_Precision.h>

#include <Geometry/CGLineSegment.h>
#include <Geometry/CGAABB.h>
#include <Geometry/CGCollisionFunction.h>

namespace CG
{
#define Node_Null -1
	struct TreeNode
	{
		bool isLeaf(void) const
		{
			// The right leaf does not use the same memory as the userdata
			return left == Node_Null;
		}

		// Fat AABB for leafs, bounding AABB for branches
		GPED::c3AABB aabb;

		union
		{
			int parent;
			int next; // free list
		};

		// Child indices
		int left;
		int right;

		void* userdata;

		// leaf = 0, free nodes = -1
		int height;
	};

	class DynamicAABBTree
	{
	public:
		DynamicAABBTree();
		DynamicAABBTree(int nodeCapacity);

		~DynamicAABBTree();

		int CreateProxy(const GPED::c3AABB& aabb, void* userData);
		void DestroyProxy(int proxyId);

		// this method is the same as MoveProxy in the box2D.
		// I changed the name for the more intuition
		bool UpdateProxy(int proxyId, const GPED::c3AABB& aabb, const glm::vec3 displacement);

		// Version with no displacement prediction
		bool UpdateProxy(int proxyId, const GPED::c3AABB& aabb);
		
		void* GetUserData(int proxyId) const;

		const GPED::c3AABB& GetFatAABB(int proxyId) const;

		template<typename T>
		void Query(T* callback, const GPED::c3AABB& aabb) const;

		template<typename T>
		void RayCast(T* callback, const GPED::c3RayInput& input) const;

		template<typename T>
		void LineSegmentCast(T* callback, const CGLineSegment& input) const;

		int GetHeight() const;

	private:
		friend class BroadRendererWrapper;

		int AllocateNode();
		void FreeNode(int nodeId);

		void InsertLeaf(int leaf);
		void RemoveLeaf(int leaf);

		s32 Balance(s32 index);

		s32 ComputeHeight(int nodeId) const;
		s32 ComputeHeight() const;

		int m_root;
		
		TreeNode* m_nodes;
		int m_nodeCount;
		int m_nodeCapacity;

		int m_freeList;

		int m_path;

		int m_insertionCount;

		void Validate();
		void ValidateStructure(int index) const;
		void ValidateMetrics(int index) const;
	};

	template<typename T>
	inline void DynamicAABBTree::Query(T * callback, const GPED::c3AABB & aabb) const
	{
		const int stackCapacity = 256;
		int stack[stackCapacity];
		stack[0] = m_root;
		
		int count = 1;
		while (count)
		{
			assert(count < stackCapacity);
			// Pop from the stack
			int nodeId = stack[--count]; 

			if (nodeId == Node_Null)
				continue;

			const TreeNode* node = m_nodes + nodeId;

			if (GPED::aabbOverlap(node->aabb, aabb))
			{
				if (node->isLeaf())
				{
					bool proceed = callback->QueryCallback(nodeId);
					if(proceed == false) 
						return;
				}
				else
				{
					stack[count++] = node->left;
					stack[count++] = node->right;
				}
			}
		}

	}
	template<typename T>
	inline void DynamicAABBTree::RayCast(T * callback, const GPED::c3RayInput & input) const
	{
		const int stackCapacity = m_nodeCount;
		int* stack = (int*)malloc(sizeof(int) * m_nodeCount);
		stack[0] = m_root;

		int count = 1;
		while (count)
		{
			assert(count < stackCapacity);
			// Pop from the stack
			int nodeId = stack[--count];

			if (nodeId == Node_Null)
				continue;

			const TreeNode* node = m_nodes + nodeId;

			if (GPED::rayaabbOverlap(node->aabb, input))
			{
				if (node->isLeaf())
				{
					bool proceed = callback->RayCastCallback(input, nodeId);
					if (proceed == false)
						break;
				}
				else
				{
					stack[count++] = node->left;
					stack[count++] = node->right;
				}
			}
		}

		free(stack);
	}

	template<typename T>
	inline void DynamicAABBTree::LineSegmentCast(T* callback, const CGLineSegment& input) const
	{
		const int stackCapacity = m_nodeCount;
		int* stack = (int*)malloc(sizeof(int) * m_nodeCount);
		stack[0] = m_root;

		int count = 1;
		while (count)
		{
			assert(count < stackCapacity);
			// Pop from the stack
			int nodeId = stack[--count];

			if (nodeId == Node_Null)
				continue;

			const TreeNode* node = m_nodes + nodeId;

			// TODO : remove GPED from DynamicAABBTree
			// c3AABB and CGAABB have same data layout
			// So we can use like this temporarily
			CGAABB cg_aabb;
			cg_aabb.m_min = CGVec3(node->aabb.min[0], node->aabb.min[1], node->aabb.min[2]);
			cg_aabb.m_max = CGVec3(node->aabb.max[0], node->aabb.max[1], node->aabb.max[2]);
			
			if (Intersect(cg_aabb, input))
			{
				if (node->isLeaf())
				{
					bool proceed = callback->LineSegmentCastCallback(input, nodeId);
					if (proceed == false)
						break;
				}
				else
				{
					stack[count++] = node->left;
					stack[count++] = node->right;
				}
			}
		}

		free(stack);
	}
}

#endif