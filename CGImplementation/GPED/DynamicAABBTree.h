#pragma once
#ifndef __DYNAMIC_AABB_TREE_H__
#define __DYNAMIC_AABB_TREE_H__



#include <glm/common.hpp>




namespace CGProj
{
	/* 18-11-13 Chanhaneg Lee
		I referred to http://www.randygaul.net/2013/08/06/dynamic-aabb-tree/ .
		However, this article is almost based on the Box2D Code.
		So, I also referred to the Box2D Code.
		I made the code work with our project code
	*/
	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
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
		AABB aabb;

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


	/*
		This Dynamic AABB Tree is implemented with statif linked list
		It means this tree will not allocate memory at runtime
	*/
	class DynamicAABBTree
	{
	public:
		DynamicAABBTree();

		~DynamicAABBTree();

		int CreateProxy(const AABB& aabb, void* userData);
		void DestroyProxy(int proxyId);
		
		void* GetUserData(int proxyId) const;

		const AABB& GetFatAABB(int proxyId) const;

		template<typename T>
		void Query(T* callback, const AABB& aabb) const;

		int GetHiehgt() const;

	private:
		int AllocateNode();
		void FreeNode(int node);

		void InsertLeaf(int node);
		void RemoveLeaf(int node);

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
	inline void DynamicAABBTree::Query(T * callback, const AABB & aabb) const
	{
	}
}



#endif