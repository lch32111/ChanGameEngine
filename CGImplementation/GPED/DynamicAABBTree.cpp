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

CGProj::DynamicAABBTree::~DynamicAABBTree()
{
}

int CGProj::DynamicAABBTree::CreateProxy(const AABB & aabb, void * userData)
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

const CGProj::AABB& CGProj::DynamicAABBTree::GetFatAABB(int proxyId) const
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

void CGProj::DynamicAABBTree::InsertLeaf(int node)
{
}

void CGProj::DynamicAABBTree::RemoveLeaf(int node)
{
}

int CGProj::DynamicAABBTree::Balance(int index)
{
	return 0;
}
