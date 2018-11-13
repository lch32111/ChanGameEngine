#include "DynamicAABBTree.h"

CGProj::DynamicAABBTree::DynamicAABBTree()
{
	m_root = Node_Null;

	m_nodeCapacity = 16;
	m_nodeCount = 0;
	m_nodes = new TreeNode[m_nodeCapacity * sizeof(TreeNode)];

	// Build a linked list for the free list
	for (int i = 0; i < m_nodeCapacity - 1; ++i)
	{
		m_nodes[i].next = i + 1;
		m_nodes[i].height = -1;
	}
	m_nodes[m_nodeCapacity - 1].next = Node_Null;
	m_nodes[m_nodeCapacity - 1].height = -1;
	m_freeList = 0;

	m_path = 0;

	m_insertionCount = 0;
}

CGProj::DynamicAABBTree::DynamicAABBTree(int nodeCapacity)
{
	m_root = Node_Null;

	m_nodeCapacity = nodeCapacity;
	m_nodeCount = 0;
	m_nodes = new TreeNode[m_nodeCapacity * sizeof(TreeNode)];

	// Build a linked list for the free list
	for (int i = 0; i < m_nodeCapacity - 1; ++i)
	{
		m_nodes[i].next = i + 1;
		m_nodes[i].height = -1;
	}
	m_nodes[m_nodeCapacity - 1].next = Node_Null;
	m_nodes[m_nodeCapacity - 1].height = -1;
	m_freeList = 0;

	m_path = 0;

	m_insertionCount = 0;
}

CGProj::DynamicAABBTree::~DynamicAABBTree()
{
	delete[] m_nodes;
	m_nodes = nullptr;
}

int CGProj::DynamicAABBTree::CreateProxy(const c3AABB & aabb, void * userData)
{
	return 0;
}

void CGProj::DynamicAABBTree::DestroyProxy(int proxyId)
{
}

void * CGProj::DynamicAABBTree::GetUserData(int proxyId) const
{
	return nullptr;
}

const CGProj::c3AABB& CGProj::DynamicAABBTree::GetFatAABB(int proxyId) const
{
	// TODO: insert return statement here
}

int CGProj::DynamicAABBTree::GetHiehgt() const
{
	return 0;
}

int CGProj::DynamicAABBTree::AllocateNode()
{
	return 0;
}

void CGProj::DynamicAABBTree::FreeNode(int node)
{
}

void CGProj::DynamicAABBTree::InsertLeaf(int leaf)
{
	++m_insertionCount;

	if (m_root == Node_Null)
	{
		m_root = leaf;
		m_nodes[m_root].parent = Node_Null;
		return;
	}

	// Find the best sibling for this node with Surface Area Heuristic
	c3AABB leafAABB = m_nodes[leaf].aabb;
	int index = m_root;
	while(m_nodes[index].isLeaf() == false)
	{
		int left = m_nodes[index].left;
		int right = m_nodes[index].right;

		// Cost Heuristic Traversal on tree
		GPED::real area = m_nodes[index].aabb.GetPerimeter();
		c3AABB combinedAABB;
		combinedAABB.Combine(m_nodes[index].aabb, leafAABB);
		GPED::real combinedArea = combinedAABB.GetPerimeter();

		// Cost of creating a new parent for this node and the new leaf
		GPED::real cost = GPED::real(2) * combinedArea;

		// Minimum cost of pushing the leaf further down the three
		GPED::real inheritanceCost = GPED::real(2) * (combinedArea - area);

		// Cost of descending into left child
		GPED::real cost1;
		if (m_nodes[left].isLeaf())
		{
			c3AABB aabb;
			aabb.Combine(leafAABB, m_nodes[left].aabb);
			cost1 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			c3AABB aabb;
			aabb.Combine(leafAABB, m_nodes[left].aabb);
			GPED::real oldArea = m_nodes[left].aabb.GetPerimeter();
			GPED::real newArea = aabb.GetPerimeter();
			cost1 = (newArea - oldArea) + inheritanceCost;
		}

		// Cost of descending into right child
		GPED::real cost2;
		if (m_nodes[right].isLeaf())
		{
			c3AABB aabb;
			aabb.Combine(leafAABB, m_nodes[right].aabb);
			cost2 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			c3AABB aabb;
			aabb.Combine(leafAABB, m_nodes[right].aabb);
			GPED::real oldArea = m_nodes[right].aabb.GetPerimeter();
			GPED::real newArea = aabb.GetPerimeter();
			cost2 = (newArea - oldArea) + inheritanceCost;
		}

		// Descend according to the minimum cost
		if (cost < cost1 && cost < cost2)
			break;

		// Descend
		if (cost1 < cost2) index = left;
		else index = right;
	}

	int sibling = index;

	// Create a new parent
	int oldParent = m_nodes[sibling].parent;
	int newParent = AllocateNode();
	m_nodes[newParent].parent = oldParent;
	m_nodes[newParent].userdata = nullptr;
	m_nodes[newParent].aabb.Combine(leafAABB, m_nodes[sibling].aabb);
	m_nodes[newParent].height = m_nodes[sibling].height + 1;

	if (oldParent != Node_Null)
	{
		// The sibling was not the root
		if (m_nodes[oldParent].left == sibling)
		{
			m_nodes[oldParent].left = newParent;
		}
		else
		{
			m_nodes[oldParent].right = newParent;
 		}

		m_nodes[newParent].left = sibling;
		m_nodes[newParent].right = leaf;
		m_nodes[sibling].parent = newParent;
		m_nodes[leaf].parent = newParent;
	}
	else
	{
		// The sibling was the root.
		m_nodes[newParent].left = sibling;
		m_nodes[newParent].right = leaf;
		m_nodes[sibling].parent = newParent;
		m_nodes[leaf].parent = newParent;
		m_root = newParent;
	}

	// Walk back up the tree fixing heights and AABBs
	index = m_nodes[leaf].parent;
	while (index != Node_Null)
	{
		// index = Balance(index);

		int left = m_nodes[index].left;
		int right = m_nodes[index].right;

		assert(left != Node_Null);
		assert(right != Node_Null);

		m_nodes[index].height = 1 + GPED::rMax(m_nodes[left].height, m_nodes[right].height);
		m_nodes[index].aabb.Combine(m_nodes[left].aabb, m_nodes[right].aabb);

		index = m_nodes[index].parent;
	}
}

void CGProj::DynamicAABBTree::RemoveLeaf(int node)
{
}

int CGProj::DynamicAABBTree::Balance(int index)
{
	return 0;
}
