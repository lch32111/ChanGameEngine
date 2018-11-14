#pragma once
#ifndef __DYNAMIC_AABB_TREE_H__
#define __DYNAMIC_AABB_TREE_H__



#include <GPED/GPED_Precision.h>


namespace CGProj
{
#define Node_Null -1
	struct TreeNode
	{
		bool isLeaf(void) const
		{
			// The right leaf does not use the same memory as the userdata
			return right == Node_Null;
		}

		// Fat AABB for leafs, bounding AABB for branches
		GPED::c3AABB aabb;

		union
		{
			int parent;
			int next; // free list
		};

		union
		{
			// Child indices
			struct
			{
				int left;
				int right;
			};

			// Since only leaf nodes hold userdata, we can use the
			// same memory used for left/right indices to store
			// the userdata void pointer
			void* userdata;
		};

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
		
		void* GetUserData(int proxyId) const;

		const GPED::c3AABB& GetFatAABB(int proxyId) const;

		template<typename T>
		void Query(T* callback, const GPED::c3AABB& aabb) const;

		int GetHiehgt() const;

	private:
		int AllocateNode();
		void FreeNode(int nodeId);

		void InsertLeaf(int leaf);
		void RemoveLeaf(int leaf);

		int Balance(int index);

		int m_root;
		
		TreeNode* m_nodes;
		int m_nodeCount;
		int m_nodeCapacity;

		int m_freeList;

		int m_path;

		int m_insertionCount;
	};

	template<typename T>
	inline void DynamicAABBTree::Query(T * callback, const GPED::c3AABB & aabb) const
	{
		const int stackCapacity = 256;
		int stack[stackCapacity];
		int count = 0;

		stack[count] = m_root;

		while (count < stackCapacity && count >= 0)
		{
			// Pop from the stack
			int nodeId = stack[count--]; 

			if (nodeId == Node_Null)
				continue;

			const TreeNode* node = m_nodes + nodeId;

			if (aabbOverlap(node->aabb, aabb)
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
					stack[count] = node->right;
				}
			}
		}

	}
}

#endif