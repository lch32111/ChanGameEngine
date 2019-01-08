#include <GPED/CGContactManager.h>
#include "CGContactManager.h"

CGProj::CGContactManager::CGContactManager()
{
	m_root = NODE_NULL;

	m_nodeCapacity = 16;
	m_nodeCount = 0;
	m_nodes = new GPED::Contact[m_nodeCapacity];

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

CGProj::CGContactManager::CGContactManager(int nodeCapacity)
{
	m_root = NODE_NULL;

	m_nodeCapacity = nodeCapacity;
	m_nodeCount = 0;
	m_nodes = new GPED::Contact[m_nodeCapacity];

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

CGProj::CGContactManager::~CGContactManager()
{
	delete[] m_nodes;
	m_nodes = nullptr;
}

int CGProj::CGContactManager::GetEmptyContactNode()
{
	int nodeId = AllocateNode();
	InsertNode(nodeId);
	return nodeId;
}

void CGProj::CGContactManager::setBodyData(int nodeId, GPED::RigidBody * one, GPED::RigidBody * two)
{
	GPED::Contact* theContact = &m_nodes[nodeId];
	theContact->body[0] = one;
	theContact->body[1] = two;
	theContact->friction = friction;
	theContact->restitution = restitution;

	int index = 0;
	if (one)
	{
		int move = one->contacts;
		if (move != NODE_NULL)
		{
			index = (m_nodes[move].body[0] == one) ? 0 : 1;
			while (m_nodes[move].nextObjects[index] != NODE_NULL)
			{
				move = m_nodes[move].nextObjects[index];
				index = (m_nodes[move].body[0] == one) ? 0 : 1;
			}

			m_nodes[move].nextObjects[index] = nodeId;
		}
		else
		{
			one->contacts = nodeId;
		}
	}

	if (two)
	{
		int move = two->contacts;
		if (move != NODE_NULL)
		{
			index = (m_nodes[move].body[0] == two) ? 0 : 1;
			while (m_nodes[move].nextObjects[index] != NODE_NULL)
			{
				move = m_nodes[move].nextObjects[index];
				index = (m_nodes[move].body[0] == two) ? 0 : 1;
			}

			m_nodes[move].nextObjects[index] = nodeId;
		}
		else
		{
			two->contacts = nodeId;
		}
	}
}

GPED::Contact * CGProj::CGContactManager::GetMaxPenetration()
{
	int move = m_root;
	int index = m_root;
	GPED::real max = -1;
	while (move != NODE_NULL)
	{
		if (max < m_nodes[move].penetration)
		{
			max = m_nodes[move].penetration;
			index = move;
		}
		move = m_nodes[move].next;
	}
	return &(m_nodes[index]);
}

GPED::Contact * CGProj::CGContactManager::GetMaxVelocity()
{
	int move = m_root;
	int index = m_root;
	GPED::real max = -1;
	while (move != NODE_NULL)
	{
		if (max < m_nodes[move].desiredDeltaVelocity)
		{
			max = m_nodes[move].desiredDeltaVelocity;
			index = move;
		}
		move = m_nodes[move].next;
	}
	return &(m_nodes[index]);
}

void CGProj::CGContactManager::AllcalculateInternals(GPED::real duration)
{
	int move = m_root;
	while (move != NODE_NULL)
	{
		m_nodes[move].calculateInternals(duration);
		move = m_nodes[move].next;
	}
}

GPED::Contact * CGProj::CGContactManager::GetFirstContact()
{
	return &m_nodes[m_root];
}

int CGProj::CGContactManager::GetNodeCount()
{
	return m_nodeCount;
}

void CGProj::CGContactManager::reset()
{
	int move = m_root;
	while (move != NODE_NULL)
	{
		int freeId = move;
		move = m_nodes[move].next;
		DeleteNode(freeId);
	}
}

int CGProj::CGContactManager::AllocateNode()
{
	if (m_freeList == NODE_NULL)
	{
		assert(m_nodeCount == m_nodeCapacity);

		// The free list is empty. Rebuild a bigger pool
		GPED::Contact* oldNodes = m_nodes;
		m_nodeCapacity *= 2;
		m_nodes = new GPED::Contact[m_nodeCapacity];
		memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(GPED::Contact));
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
	m_nodes[nodeId].nextObjects[0] = NODE_NULL;
	m_nodes[nodeId].nextObjects[1] = NODE_NULL;
	m_nodes[nodeId].myId = nodeId;
	++m_nodeCount;

	return nodeId;
}

void CGProj::CGContactManager::FreeNode(int nodeId)
{
	assert(0 <= nodeId && nodeId < m_nodeCapacity);
	assert(0 < m_nodeCount);
	m_nodes[nodeId].next = m_freeList;
	m_nodes[nodeId].body[0]->contacts = NODE_NULL;
	if(m_nodes[nodeId].body[1])
		m_nodes[nodeId].body[1]->contacts = NODE_NULL;
	m_nodes[nodeId].body[0] = nullptr;
	m_nodes[nodeId].body[1] = nullptr;
	m_nodes[nodeId].myId = NODE_NULL;
	m_nodes[nodeId].nextObjects[0] = NODE_NULL;
	m_nodes[nodeId].nextObjects[1] = NODE_NULL;

	m_freeList = nodeId;
	--m_nodeCount;
}

void CGProj::CGContactManager::InsertNode(int nodeId)
{
	if (m_root == NODE_NULL)
	{
		m_root = nodeId;
		m_nodes[m_root].prev = NODE_NULL;
		m_nodes[m_root].next = NODE_NULL;
		return;
	}

	// Insert the node into the first place of doubly linked list
	m_nodes[nodeId].prev = NODE_NULL;
	m_nodes[nodeId].next = m_root;

	m_nodes[m_root].prev = nodeId;

	m_root = nodeId;
}

void CGProj::CGContactManager::DeleteNode(int nodeId)
{
	int prevId = m_nodes[nodeId].prev;
	int nextId = m_nodes[nodeId].next;

	if (prevId != NODE_NULL)
		m_nodes[prevId].next = nextId;
	else
		m_root = nextId;

	if(nextId != NODE_NULL)
		m_nodes[nextId].prev = prevId;

	FreeNode(nodeId);
}