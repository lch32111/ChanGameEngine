#pragma once
#ifndef __CG_CONTACT_MANAGER_H__
#define __CG_CONTACT_MANAGER_H__

#include <GPED/GPED_contacts.h>

namespace GPED
{
	/* 181119 Chanhaeng Lee
	* The overall structure of classes used in this contact hedaer is like..
	* Contact == Node
	* Contact Manager == Doubly linked list managing inserting, removing, allocating, freeing, sorting
	*	                 Contact Manager will provide the function to help speed up the resolution proceess
	*					 based on the GPED book optimization tip.
	* Contact Resolver == Contact Manager sends the contacts to resolve
	*
	* + the relation with narrow phase dtector
	* the narrow phase detector interacts with Contact Manager.
	* For Example, GetEmptyContactNode();
	*/

#define NODE_NULL -1
	class ContactManager
	{
	public:
		ContactManager();
		ContactManager(int nodeCapacity);
		~ContactManager();

		Contact* GetEmptyContactNode();

		void sortByPenetration();
		void sortByVelocity();
	private:
		int AllocateNode();
		void FreeNode(int nodeId);

		void InsertNode(Contact* node);
		void DeleteNode(Contact* node);

		int m_root;

		Contact* m_nodes;
		int m_nodeCount;
		int m_nodeCapacity;

		int m_freeList;
	};
}


#endif