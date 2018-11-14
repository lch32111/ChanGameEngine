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

// Create a proxy in the tree as a leaf node. We return the index
// of the node instead of a pointer so that we can grow
// the node pool
int CGProj::DynamicAABBTree::CreateProxy(const c3AABB & aabb, void * userData)
{
	int proxyId = AllocateNode();

	// Fatten the aabb.
	glm::vec3 r(aabbExtension);
	m_nodes[proxyId].aabb.min = aabb.min - r;
	m_nodes[proxyId].aabb.max = aabb.max + r;
	m_nodes[proxyId].userdata = userData;
	m_nodes[proxyId].height = 0;

	InsertLeaf(proxyId);

	return proxyId;
}

void CGProj::DynamicAABBTree::DestroyProxy(int proxyId)
{
	assert(0 <= proxyId && proxyId < m_nodeCapacity);
	assert(m_nodes[proxyId].isLeaf());

	RemoveLeaf(proxyId);
	FreeNode(proxyId);
}

bool CGProj::DynamicAABBTree::UpdateProxy(int proxyId, const c3AABB & aabb, const glm::vec3 displacement)
{
	assert(0 <= proxyId && proxyId < m_nodeCapacity);
	assert(m_nodes[proxyId].isLeaf());

	if (m_nodes[proxyId].aabb.Contains(aabb))
		return false;

	RemoveLeaf(proxyId);

	// Extend AABB
	c3AABB b = aabb;
	glm::vec3 r(aabbExtension);
	b.min = b.min - r;
	b.max = b.max + r;

	// Predict AABB displacement
	glm::vec3 d = displacement * GPED::real(aabbMultiplier);

	if (d.x < GPED::real(0.0)) b.min.x += d.x;
	else b.max.x += d.x;

	if (d.y < GPED::real(0.0)) b.min.y += d.y;
	else b.max.y += d.y;

	if (d.z < GPED::real(0.0)) b.min.z += d.z;
	else b.max.z += d.z;

	m_nodes[proxyId].aabb = b;

	InsertLeaf(proxyId);
	return true;
}

void * CGProj::DynamicAABBTree::GetUserData(int proxyId) const
{
	assert(0 <= proxyId && proxyId < m_nodeCapacity);
	return m_nodes[proxyId].userdata;
}

const CGProj::c3AABB& CGProj::DynamicAABBTree::GetFatAABB(int proxyId) const
{
	assert(0 <= proxyId && proxyId < m_nodeCapacity);
	return m_nodes[proxyId].aabb;
}

int CGProj::DynamicAABBTree::GetHiehgt() const
{
	if(m_root == Node_Null)
		return 0;
	
	return m_nodes[m_root].height;
}

// Allocate a node from the pool. Grow the pool if necessary
int CGProj::DynamicAABBTree::AllocateNode()
{
	// Expand the node pool as needed.
	if (m_freeList == Node_Null)
	{
		assert(m_nodeCount == m_nodeCapacity);

		// The free list is empty. Rebuild a bigger pool
		TreeNode* oldNodes = m_nodes;
		m_nodeCapacity *= 2;
		m_nodes = new TreeNode[m_nodeCapacity];
		memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(TreeNode));

		// Build a linked list for the free list. The parent
		// pointer becomes the "next" pointer.
		for (int i = m_nodeCount; i < m_nodeCapacity - 1; ++i)
		{
			m_nodes[i].next = i + 1;
			m_nodes[i].height = -1;
		}
		m_nodes[m_nodeCapacity - 1].next = Node_Null;
		m_nodes[m_nodeCapacity - 1].height = -1;
		m_freeList = m_nodeCount;
	}

	// Peel a node off the free list
	int nodeId = m_freeList;
	m_freeList = m_nodes[nodeId].next;
	m_nodes[nodeId].parent = Node_Null;
	m_nodes[nodeId].left = Node_Null;
	m_nodes[nodeId].right = Node_Null;
	m_nodes[nodeId].height = 0;
	m_nodes[nodeId].userdata = nullptr;
	++m_nodeCount;
	return nodeId;
}

// Return a node to the pool
void CGProj::DynamicAABBTree::FreeNode(int nodeId)
{
	assert(0 <= nodeId && nodeId < m_nodeCapacity);
	assert(0 < m_nodeCount);
	m_nodes[nodeId].next = m_freeList;
	m_nodes[nodeId].height = -1;
	m_freeList = nodeId;
	--m_nodeCount;
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

void CGProj::DynamicAABBTree::RemoveLeaf(int leaf)
{
	if (leaf == m_root)
	{
		m_root = Node_Null;
		return;
	}

	int parent = m_nodes[parent].parent;
	int grandParent = m_nodes[parent].parent;
	int sibling;
	if (m_nodes[parent].left == leaf)
		sibling = m_nodes[parent].right;
	else
		sibling = m_nodes[parent].left;

	// parent is not the root.
	if (grandParent != Node_Null)
	{
		// Destroy parent and connect sibling to grandParnet
		if (m_nodes[grandParent].left == parent)
			m_nodes[grandParent].left = sibling;
		else
			m_nodes[grandParent].right = sibling;

		m_nodes[sibling].parent = grandParent;
		FreeNode(parent);

		// Adjust ancestor bounds
		int index = grandParent;
		while (index != Node_Null)
		{
			// index = Balance(index);
			
			int left = m_nodes[index].left;
			int right = m_nodes[index].right;

			m_nodes[index].aabb.Combine(m_nodes[left].aabb, m_nodes[right].aabb);
			m_nodes[index].height = 1 + GPED::rMax(m_nodes[left].height, m_nodes[right].height);

			index = m_nodes[index].parent;
		}
	}
	// parent is the root.
	else
	{
		m_root = sibling;
		m_nodes[sibling].parent = Node_Null;
		FreeNode(parent);
	}
		 
}

int CGProj::DynamicAABBTree::Balance(int index)
{
	return 0;
}

bool CGProj::DynamicAABBTree::aabbOverlap(const c3AABB & a, const c3AABB & b)
{
	// Exit with no intersection if separated along an axis
	if (a.max[0] < b.min[0] || a.min[0] > b.max[0]) return false;
	if (a.max[1] < b.min[1] || a.min[1] > b.max[1]) return false;
	if (a.max[2] < b.min[2] || a.min[2] > b.max[2]) return false;
	
	// Overlapping on all axes means AABBs are intersecting
	return true;
}
