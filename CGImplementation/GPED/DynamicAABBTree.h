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


	/* 18-11-13 Chanhaeng Lee
		This Dynamic AABB Tree is implemented with statif linked list
		It means this tree will not allocate memory at runtime.
		If you want, then you can implement your own growable array similar to std::vector.
		But you should note that you avoid allocating memory frequenrly at runtime.
	*/
	class DynamicAABBTree
	{
	public:
		DynamicAABBTree();
		DynamicAABBTree(int nodeCapacity);

		~DynamicAABBTree();

		int CreateProxy(const c3AABB& aabb, void* userData);
		void DestroyProxy(int proxyId);
		
		void* GetUserData(int proxyId) const;

		const c3AABB& GetFatAABB(int proxyId) const;

		template<typename T>
		void Query(T* callback, const c3AABB& aabb) const;

		int GetHiehgt() const;

	private:
		int AllocateNode();
		void FreeNode(int node);

		void InsertLeaf(int leaf);
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
	inline void DynamicAABBTree::Query(T * callback, const c3AABB & aabb) const
	{
	}
}



#endif