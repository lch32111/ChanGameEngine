#ifndef __GPED_WORLD_H__
#define __GPED_WORLD_H__

#include <GPED/GPED_body.h>
#include <GPED/GPED_contacts.h>
#include <GPED/GPED_ContactResolver.h>

/*
	!!! NOTICE !!!
	THIS CLASS WILL NOT WORK WELL!
	BECAUSE I didn't modify this class for lont time after changing lots of physics systems
*/

namespace GPED
{
	/**
	* The world represents an independent simulation of physics.  It
	* keeps track of a set of rigid bodies, and provides the means to
	* update them all.
	*/
	class World
	{
		// ... other World data as before ...
		/**
		* True if the world should calculate the number of iterations
		* to give the contact resolver at each frame.
		*/
		bool calculateIterations;

		/**
		* Holds a single rigid body in a linked list of bodies.
		*/
		struct BodyRegistration
		{
			RigidBody* body;
			BodyRegistration* next;
		};

		/**
		* Holds the head of the list of registered bodies.
		*/
		BodyRegistration* firstBody;

		/**
		 * Holds the resolver for sets of contacts.
		 */
		ContactResolver resolver;

		/**
		 * Holds one contact generators in a linked list.
		 */
		struct ContactGenRegistration
		{
			ContactGenerator* gen;
			ContactGenRegistration* next;
		};

		/**
		 * Holds the head of the list of contact generators.
		 */
		ContactGenRegistration* firstContactGen;

		/** 
		 * Holds an array of contacts, for filling by the contact
		 * generators.
		 */
		Contact* contacts;

		/**
		 * Holds the maximum number of contacts allowed (i.e., the size
		 * of the contacts array).
		 */
		unsigned maxContacts;


	
	public:
		/**
		 * Creates a new simulator that can handle up to the given 
		 * number of contacts per frame. You can also optionally give 
		 * a number of contact-resolution iterations to use. If you
		 * don't give a number of iterations, then four times the
		 * number of detected contacts will be used for each frame.
		 */
		World(unsigned maContacts, unsigned iterations = 0);
		~World();

		/**
		 * Calls each of the registered contact generators to report
		 * their contacts. Returns the number of generated contacts.
		 */
		unsigned generateContacts();

		/**
		* Processes all the physics for the world.
		*/
		void runPhysics(real duration);

		/**
		* Initialises the world for a simulation frame. This clears
		* the force and torque accumulators for bodies in the
		* world. After calling this, the bodies can have their forces
		* and torques for this frame added.
		*/
		void startFrame();
	};
}

#endif