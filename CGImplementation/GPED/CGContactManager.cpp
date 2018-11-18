#include "CGContactManager.h"
#pragma once

GPED::ContactManager::ContactManager()
{
	m_root = NODE_NULL;

	m_nodeCapacity = 16;
	m_nodeCount = 0;
	m_nodes = new Contact[m_nodeCapacity];

	// Build a linked list for the free list
	m_nodes[0].prev = NODE_NULL;
	m_nodes[0].next = 1;
	for (int i = 1; i < m_nodeCapacity - 1; ++i)
	{
		m_nodes[i].prev = i - 1;
		m_nodes[i].next = i + 1;
	}
	m_nodes[m_nodeCapacity - 1].prev = m_nodeCapacity - 2;
	m_nodes[m_nodeCapacity - 1].next = NODE_NULL;
	
	m_freeList = 0;
}

GPED::ContactManager::ContactManager(int nodeCapacity)
{
	m_root = NODE_NULL;

	m_nodeCapacity = nodeCapacity;
	m_nodeCount = 0;
	m_nodes = new Contact[m_nodeCapacity];

	// Build a linked list for the free list
	m_nodes[0].prev = NODE_NULL;
	m_nodes[0].next = 1;
	for (int i = 1; i < m_nodeCapacity - 1; ++i)
	{
		m_nodes[i].prev = i - 1;
		m_nodes[i].next = i + 1;
	}
	m_nodes[m_nodeCapacity - 1].prev = m_nodeCapacity - 2;
	m_nodes[m_nodeCapacity - 1].next = NODE_NULL;

	m_freeList = 0;
}

GPED::ContactManager::~ContactManager()
{
	delete[] m_nodes;
	m_nodes = nullptr;
}

GPED::Contact* GPED::ContactManager::GetEmptyContactNode()
{
	return nullptr;
}

void GPED::ContactManager::sortByPenetration()
{
}

void GPED::ContactManager::sortByVelocity()
{
}

int GPED::ContactManager::AllocateNode()
{
	if (m_freeList == NODE_NULL)
	{
		assert(m_nodeCount == m_nodeCapacity);

		// The free list is empty. Rebuild a bigger pool
		Contact* oldNodes = m_nodes;
		m_nodeCapacity *= 2;
		m_nodes = new Contact[m_nodeCapacity];
		memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(Contact));
		delete[] oldNodes;
		oldNodes = nullptr;

		// Build a linked list for the free list.
		m_nodes[m_nodeCount].prev = NODE_NULL;
		m_nodes[m_nodeCount].next = m_nodeCount + 1;
		for (int i = m_nodeCount + 1; i < m_nodeCapacity - 1; ++i)
		{
			m_nodes[i].prev = NODE_NULL;
			m_nodes[i].next = i + 1;
		}
		m_nodes[m_nodeCapacity - 1].prev = m_nodeCapacity - 2;
		m_nodes[m_nodeCapacity - 1].next = NODE_NULL;
		m_freeList = m_nodeCount;
	}

	// Peel a node off the free list
	int nodeId = m_freeList;
	m_freeList = m_nodes[nodeId].next;
	m_nodes[nodeId].body[0] = nullptr;
	m_nodes[nodeId].body[1] = nullptr;
	++m_nodeCount;
	return nodeId;
}

void GPED::ContactManager::FreeNode(int nodeId)
{
	assert(0 <= nodeId && nodeId < m_nodeCapacity);
	assert(0 < m_nodeCount);
	m_nodes[nodeId].next = m_freeList;
	m_freeList = nodeId;
	--m_nodeCount;
}

void GPED::ContactManager::InsertNode(Contact * node)
{
	if (m_root = NODE_NULL)
	{
		m_root = 0;
		m_nodes[]
	}
}

void GPED::ContactManager::DeleteNode(Contact * node)
{
}
