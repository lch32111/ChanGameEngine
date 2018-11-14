#pragma once
#ifndef __DYNAMIC_AABB_TREE_H__
#define __DYNAMIC_AABB_TREE_H__



#include <GPED/GPED_Precision.h>


namespace CGProj
{
	/* 18-11-13 Chanhaneg Lee
		I referred to http://www.randygaul.net/2013/08/06/dynamic-aabb-tree/ .
		However, this article is almost based on the Box2D Code.
		So, I also referred to the Box2D Code.
		I made the code work with our project code
	*/
#define aabbExtension 0.1
#define aabbMultiplier 2.0
	struct c3AABB // CG Project 3-dimenstional AABB
	{
		glm::vec3 min;
		glm::vec3 max;

		GPED::real GetPerimeter() const
		{
			GPED::real wx = max.x - min.x;
			GPED::real wy = max.y - min.y;
			GPED::real wz = max.z - min.z;

			return GPED::real(4) * (wx + wy + wz);
		}

		void Combine(const c3AABB& aabb)
		{
			min = GPED::rMin(min, aabb.min);
			max = GPED::rMax(max, aabb.max);
		}

		void Combine(const c3AABB& aabb1, const c3AABB& aabb2)
		{
			min = GPED::rMin(aabb1.min, aabb2.min);
			max = GPED::rMax(aabb1.max, aabb2.max);
		}

		// Does this aabb contain the provided AABB;
		bool Contains(const c3AABB& aabb) const
		{
			bool result = true;
			result = result && min.x <= aabb.min.x;
			result = result && min.y <= aabb.min.y;
			result = result && min.z <= aabb.min.z;
			result = result && aabb.max.x <= max.x;
			result = result && aabb.max.y <= max.y;
			result = result && aabb.max.z <= max.z;
			return result;
		}
	};

#define Node_Null -1
	struct TreeNode
	{
		bool isLeaf(void) const
		{
			// The right leaf does not use the same memory as the userdata
			return right == Node_Null;
		}

		// Fat AABB for leafs, bounding AABB for branches
		c3AABB aabb;

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

		int CreateProxy(const c3AABB& aabb, void* userData);
		void DestroyProxy(int proxyId);

		// this method is the same as MoveProxy in the box2D.
		// I changed the name for the more intuition
		bool UpdateProxy(int proxyId, const c3AABB& aabb, const glm::vec3 displacement);
		
		void* GetUserData(int proxyId) const;

		const c3AABB& GetFatAABB(int proxyId) const;

		template<typename T>
		void Query(T* callback, const c3AABB& aabb) const;

		int GetHiehgt() const;

	private:
		int AllocateNode();
		void FreeNode(int nodeId);

		void InsertLeaf(int leaf);
		void RemoveLeaf(int leaf);

		int Balance(int index);

		bool aabbOverlap(const c3AABB& a, const c3AABB& b);

		int m_root;
		
		TreeNode* m_nodes;
		int m_nodeCount;
		int m_nodeCapacity;

		int m_freeList;

		int m_path;

		int m_insertionCount;
	};

	template<typename T>
	inline void DynamicAABBTree::Query(T * callback, const c3AABB & aabb) const
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