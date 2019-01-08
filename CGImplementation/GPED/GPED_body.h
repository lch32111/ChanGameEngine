#ifndef __GPED_BODY_H__
#define __GPED_BODY_H__

#include <GPED/GPED_Precision.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Forward Declaration of accessing RigidBody
namespace CGProj { class CGContactManager; }

namespace GPED
{
	/**
	* A rigid body is the basic simulation object in the physics
	* core.
	*
	* It has position and orientation data, along with first
	* derivatives. It can be integrated forward through time, and
	* have forces, torques and impulses (linear or angular) applied
	* to it. The rigid body manages its state and allows access
	* through a set of methods.
	*
	* A ridid body contains 64 words (the size of which is given
	* by the precision: sizeof(real)). It contains no virtual
	* functions, so should take up exactly 64 words in memory. Of
	* this total 15 words are padding, distributed among the
	* Vector3 data members.
	*/
	class RigidBody
	{
	public:
		// ... Other RigidBody code as before ...
	protected:
		/**
		* @name Characteristic Data and State
		*
		* This data holds the state of the rigid body. There are two
		* sets of data: characteristics and state.
		*
		* Characteristics are properties of the rigid body
		* independent of its current kinematic situation. This
		* includes mass, moment of inertia and damping
		* properties. Two identical rigid bodys will have the same
		* values for their characteristics.
		*
		* State includes all the characteristics and also includes
		* the kinematic situation of the rigid body in the current
		* simulation. By setting the whole state data, a rigid body's
		* exact game state can be replicated. Note that state does
		* not include any forces applied to the body. Two identical
		* rigid bodies in the same simulation will not share the same
		* state values.
		*
		* The state values make up the smallest set of independent
		* data for the rigid body. Other state data is calculated
		* from their current values. When state data is changed the
		* dependent values need to be updated: this can be achieved
		* either by integrating the simulation, or by calling the
		* calculateInternals function. This two stage process is used
		* because recalculating internals can be a costly process:
		* all state changes should be carried out at the same time,
		* allowing for a single call.
		*
		* @see calculateInternals
		*/

		/**
		* Holds the inverse of the mass of the rigid body. It
		* is more useful to hold the inverse mass because
		* integration is simpler, and because in real time
		* simulation it is more useful to have bodies with
		* infinite mass (immovable) than zero mass
		* (completely unstable in numerical simulation).
		*/
		real inverseMass;

		/**
		 * Holds the inverse of the body's inertia tensor. The
		 * inertia tensor provided must not be degenerate
		 * (that would mean the body had zero inertia for
		 * spinning along one axis). As long as the tensor is
		 * finitie, it will be invertible. The inverse tensor
		 * is used for similar reasons to the use of inverse mass
		 *
		 * The inertia tensor, unlike the other variables that
		 * define a rigid body, is given in body space.
		 * 
		 * @see inverseMass
		 */
		glm::mat3 inverseInertiaTensor;

		/**
		 * Holds the amount of damping applied to linear
		 * motion. Damping is required to remove energy added
		 * through numerical instability in the integrator.
		 */
		real linearDamping;

		/** 
		 * Holds the amount of damping applied to angular
		 * motion. Damping is required to remove energy added
		 * through numerical instability in the integrator.
		 */
		real angularDamping;

		/**
		 * Holds the linear position of the rigid body in world space
		 */
		glm::vec3 position;

		/**
		 * Holds the linear position of the rigid body in world space 
		 * from lastFrame. This is for the broadPhase update of predicting the aabb move
		 */
		glm::vec3 lastFramePosition;

		/**
		 * Holds the angular orientation of the rigid body in 
		 * world space
		 */
		glm::quat orientation;

		/**
		 * Holds the linear velocity of the rigid body in
		 * world space
		 */
		glm::vec3 velocity;

		/**
		 * Holds the angular velocity, or rotation, of the
		 * rigid body in world space.
		 */
		glm::vec3 rotation;

		/**
		 * @name Derived Data
		 * 
		 * These data members holds information that is derived from
		 * the other data in the class.
		 */

		/**
		 * Holds the inverse inertia tensor of the body in world
		 * space. The inverse inertiat ensor member is specified in 
		 * the body's local space.
		 * 
		 * @see inverseInertiaTensor
		 */
		glm::mat3 inverseInertiaTensorWorld;

		/**
		 * Holds the amount of motion of the body. This is a recency
		 * weighted mean that can be used to put a body to sleep.
		 */
		real motion;

		/**
		 * A body can be put to sleep to avoid it being updated
		 * by the integration functions or affected by collisions
		 * with the world.
		 */
		bool isAwake;

		/**
		 * Some bodies may never be allowed to fall asleep.
		 * User controlled bodies, for example, should be
		 * always awake.
		 */
		bool canSleep;

		/**
		 * Holds a transform matrix for converting body space into
		 * world space and vice versa This can be achieved by calling
		 * the getPointIn * Space functions
		 *
		 * @see getPointInLocalSpace
		 * @see getPointInWorldSpace
		 * @see getTransform
		 */
		glm::mat4 transformMatrix;

		/**
		 * @name Force and Torque Accumulators
		 * These data members store the current force, torque and
		 * acceleration of the rigid body. Forces can be added to the
		 * rigid body in any order, and the class decomposes them into
		 * their constituents, accumulating for the next
		 * simulation step. At the simulation step, the accelerations
		 * are calculated and stored to be applied to the rigid body.
		 */

		/**
		 * Holds the accumulated force to be applied at the next 
		 * integration step.
		 */
		glm::vec3 forceAccum;

		/**
		 * Holds the accumulated torque to be applied at the next
		 * integration step.
		 */
		glm::vec3 torqueAccum;

		/**
		 * Holds the acceleration of the rigid body. This value
		 * can be used to set acceleration due to gravity (its primary
		 * use), or any other constant acceleration.
		 */
		glm::vec3 acceleration;

		/**
		 * Holds the linear acceleration of the rigid body, for the
		 * previous frame
		 */
		glm::vec3 lastFrameAcceleration;

	public:
		/**
		 * @nmae Constructor and Destructor
		 * 
		 * There are no data members in the rigid body class that are
		 * created on the heap, So all data storage is handled
		 * automatically.
		 */

		/**
		 * @name Integration and Simulation Functions
		 * These functions are used to simulate the rigid body's
		 * motion over time. A normal application sets up one or more
		 * rigid bodies, applies permanent force (i.e. gravity), then
		 * adds transient forces each frame, and integrates, prior to rendering
		 *
		 * Currently the only integration function provided is the
		 * first order Newton Euler method
		 */

		/**
		 * Calculate internal data from state data. This should be called
		 * after the body's state is altered directly (it is called
		 * automatically during integration). If you change the body's state
		 * and then intend to integrate before querying any data (such as
		 * the transform matrix); then you can ommit this step
		 */
		void calculateDerivedData();

		/** 
		 * Integrates the rigid body forward in time by the given amount.
		 * This function uses a Newton-Euler integration method, which is a
		 * linear approximation to the correct integral. For this reason it
		 * may be inaccurate in some cases.
		 */
		void integrate(real duration);

		/**
		 * @name Accessor Functions for the Rigid Body's state
		 * 
		 * These functions provide access to the rigid body's
		 * characteristics or state. These data can be accessed
		 * individually, or as masses as an array of values
		 * (e.g. getCharacteristics, getState). When setting new data,
		 * make sure the calculateInternals function, or an
		 * integration routine, is called before trying to get data
		 * from the body, since the class contains a number of 
		 * dependent values that will need recalculating.
		 */


		/**
		* Sets the mass of the rigid body.
		*
		* @param mass The new mass of the body. This may not be zero.
		* Small masses can produce unstable rigid bodies under
		* simulation.
		*
		* @warning This invalidates internal data for the rigid body.
		* Either an integration function, or the calculateInternals
		* function should be called before trying to get any settings
		* from the rigid body.
		*/
		void setMass(const real mass);

		/**
		* Gets the mass of the rigid body.
		*
		* @return The current mass of the rigid body.
		*/
		real getMass() const;

		/**
		* Sets the inverse mass of the rigid body.
		*
		* @param inverseMass The new inverse mass of the body. This
		* may be zero, for a body with infinite mass
		* (i.e. unmovable).
		*
		* @warning This invalidates internal data for the rigid body.
		* Either an integration function, or the calculateInternals
		* function should be called before trying to get any settings
		* from the rigid body.
		*/
		void setInverseMass(const real inverseMass);

		/**
		* Gets the inverse mass of the rigid body.
		*
		* @return The current inverse mass of the rigid body.
		*/
		real getInverseMass() const;

		/**
		* Returns true if the mass of the body is not-infinite.
		*/
		bool hasFiniteMass() const;

		/**
		* Sets the intertia tensor for the rigid body.
		*
		* @param inertiaTensor The inertia tensor for the rigid
		* body. This must be a full rank matrix and must be
		* invertible.
		*
		* @warning This invalidates internal data for the rigid body.
		* Either an integration function, or the calculateInternals
		* function should be called before trying to get any settings
		* from the rigid body.
		*/
		void setInertiaTensor(const glm::mat3& inertiaTensor);

		/**
		* Copies the current inertia tensor of the rigid body into
		* the given matrix.
		*
		* @param inertiaTensor A pointer to a matrix to hold the
		* current inertia tensor of the rigid body. The inertia
		* tensor is expressed in the rigid body's local space.
		*/
		void getInertiaTensor(glm::mat3& inertiaTensor) const;

		/**
		 * Gets a copy of the current inertia tensor of the rigid body
		 *
		 * @return A new matrix containing the current inertia
		 * tensor. The inertia tensor is expressed in the rigid body's local space
		 */
		glm::mat3 getInertiaTensor() const;

		/**
		 * Copies the current inertia tensor of the rigid body into
		 * the given matrix
		 *
		 * @param inertiaTensor A pointer to a matrix to hold the
		 * current inertia tensor of the rigid body. The inertia
		 * tensor is expresseed in world space.
		 */
		void getInertiaTensorWorld(glm::mat3* inertiaTensor) const;

		/**
		 * Gets a copy of the current inertia tensor of the rigid body
		 *
		 * @return A new matrix containing the current inertia
		 * tensor. The inertia tensor is expressed in world space.
		 */
		glm::mat3 getInertiaTensorWorld() const;

		/**
		 * Sets the inverse inertia tensor for the rigid body.
		 * 
		 * @param inverseInertiaTensor The inverse inertia tensor for
		 * the rigid body. This must be a full rank matrix and must be
		 * invertible
		 *
		 * @warning This invalidates internal data for the rigid body.
		 * Either an integration function, or the calculateInternals
		 * funciton should be called before trying to get any setting
		 * from the rigid body
		 */
		void setInverseInertiaTensor(const glm::mat3& inverseInertiaTensor);

		/**
		 * Copies the current inverse inertia tensor of the rigid body
		 * into the given matrix
		 *
		 * @param inverseInertiaTensor A pointer to a matrix to hold
		 * the current inverse inertia tensor of the rigid body. The
		 * inertia tensor is expressed in the rigid body's local
		 * space.
		 */
		void getInverseInertiaTensor(glm::mat3* inverseInertiaTensor) const;

		/**
		 * Gets a copy of the current inverse inertia tensor of the
		 * rigid body.
		 *
		 * @return A new matrix containing the current inverse
		 * inertia tensor. The inertia tensor is expressed in the
		 * rigid body's local space.
		 */
		glm::mat3 getInverseInertiaTensor() const;

		/**
		 * Copies the current inverse inertia tensor of the rigid body
		 * into the given matrix.
		 *
		 * @param inverseInertiaTensor A pointer to a matrix to hold
		 * the current inverse inertia tensor of the rigid body.
		 * The inertia tensor is expressed in world space.
		 */
		void getInverseInertiaTensorWorld(glm::mat3* inverseInertiaTensor) const;

		/**
		 * Gets a copy of the current inverse inertia tensor of the
		 * rigid body.
		 * 
		 * @return A new matrix containing the current inverse
		 * inertia tensor. The inertia tensor is expressed in world
		 * space.
		 */
		glm::mat3 getInverseInertiaTensorWorld() const;

		/**
		 * Sets both linear and angular damping in one function call.
		 *
		 * @param linearDamping The speed that velocity is shed from
		 * the rigid body.
		 *
		 * @param angularDamping The speed that rotation is shed from
		 * the rigid body
		 *
		 * @see setLinearDamping
		 * @see setAngularDamping
		 */
		void setDamping(const real linearDamping, const real angularDamping);

		/**
		 * Sets the linear damping for the rigid body.
		 *
		 * @param linearDamping The speed that velocity is shed from
		 * the rigid body
		 *
		 * @see setAngularDamping
		 */
		void setLinearDamping(const real linearDamping);

		/**
		 * Gets the current linear damping value
		 *
		 * @return The current linear damping value
		 */
		real getLinearDamping() const;

		/**
		 * Sets the angular damping for the rigid body.
		 *
		 * @param angularDamping The speed that rotation is shed from
		 * the rigid body.
		 *
		 * @see setLinearDamping
		 */
		void setAngularDamping(const real angularDamping);

		/** 
		 * Gets the current angular damping value.
		 *
		 * @return The current angular damping value.
		 */
		real getAngularDamping() const;

		/**
		 * Sets the position of the rigid body.
		 *
		 * @param position The new position of the rigid body.
		 */
		void setPosition(const glm::vec3& position);

		/**
		 * Sets the position of the rigid body by component.
		 *
		 * @param x The x coordinate of the new position of the rigid
		 * body.
		 *
		 * @param y The y coordinate of the new position of the rigid
		 * body.
		 *
		 * @param z The z coordinate of the new position of the rigid
		 * body.
		 */
		void setPosition(const real x, const real y, const real z);

		/**
		 * Fills the given vector with the position of the rigid body.
		 * @param position A pointer to a vector into which to write
		 * the position.
		 */
		void getPosition(glm::vec3* position) const;

		/**
		 * Gets the position of the rigid body.
		 *
		 * @return The position of the rigid body.
		 */
		glm::vec3 getPosition() const;

		/**
		 * Sets the orientation of the rigid body.
		 *
		 * @param orientation The new orientation of the rigid body.
		 *
		 * @note The given orientation does not need to be normalised.
		 * and can be zero. This function autumatically constructs a
		 * valid rotation quaternion with (0,0,0,0) mapping to
		 * (1,0,0,0).
		 */
		void setOrientation(const glm::quat& orientation);

		/**
		* Sets the orientation of the rigid body by component.
		*
		* @param r The real component of the rigid body's orientation
		* quaternion.
		*
		* @param i the first complex component of the rigid body's
		* orientation quaternion.
		*
		* @param j The second complex component of the rigid body's
		* orientation quaternion.
		*
		* @param k The third complex component of the rigid body's
		* orientation quaternion.
		*
		* @note The given orientation does not need to be normalised.
		* and can be zero. This function automatically constructs a
		* valid rotation quaternion with (0,0,0,0) mapping to
		* (1,0,0,0)
		*/
		void setOrientation(const real r, const real i, const real j, const real k);

		/**
		 * Fills the given quaternion with the current value of the
		 * rigid body's orientation.
		 * 
		 * @param orientation A pointer to a quaternion to receive the
		 * orientation data.
		 */
		void getOrientation(glm::quat* orientation) const;

		/**
		 * Gets the orientation of the rigid body
		 *
		 * @return The orientation of the rigid body
		 */
		glm::quat getOrientation() const;

		/**
		 * Fills the given matrix with a transformation representing
		 * the rigid body's orientation.
		 *
		 * @note Transforming a direction vector by this matrix turns
		 * it from the body's local space to world space.
		 *
		 * @param matrix A pointer to the matrix to fill.
		 */
		void getOrientation(glm::mat3* matrix) const;

		/**
		 * Fills the given matrix with a transformation representing
		 * the rigid body's position and orientation
		 * 
		 * @note Transforming a vector by this matrix turns it from
		 * the body's local space to world space.
		 *
		 * @param transform A pointer to the matrix to fill.
		 */
		void getTransform(glm::mat4* transform) const;

		/**
		 * Gets a transformation representing the rigid body's
		 * position and orientation.
		 *
		 * @note Transforming a vector by this matrix turns is from
		 * the body's local space to world space.
		 *
		 * @return The transform matrix for the rigid body.
		 */
		glm::mat4 getTransform() const;

		/**
		 * Converts the gien point from world space into the body's
		 * local space.
		 *
		 * @param point The point to conver; given in world space.
		 *
		 * @return The converted point, in local space.
		 */
		glm::vec3 getPointInLocalSpace(const glm::vec3& point) const;

		/**
		 * Converts the given point from world space into the body's
		 * local space.
		 *
		 * @param point The point to convert, given in local space.
		 *
		 * @return The converted point, in world space.
		 */
		glm::vec3 getPointInWorldSpace(const glm::vec3& point) const;

		/**
		 * Converts the given direction from world space into the
		 * body's local space.
		 *
		 * @note When a direction is converted between frames of
		 * reference, there is no translation required.
		 *
		 * @param direction The direction to convert, given in world
		 * space.
		 *
		 * @return The converted direction, in local space.
		 */
		glm::vec3 getDirectionInLocalSpace(const glm::vec3& direction) const;

		/**
		 * Converts the given direction from bodt's local space 
		 * into world space
		 * 
		 * @note When a direction is converted between frames of
		 * reference, there is no translation required.
		 *
		 * @param direction The direction to convert, given in local space.
		 *
		 * @return The converted direction, in world space.
		 */
		glm::vec3 getDirectionInWorldSpace(const glm::vec3& direction) const;

		/**
		 * Sets the velocity of the rigid body.
		 *
		 * @param velocity The new velocity of the rigid body. The
		 * velocity is given in world space.
		 */
		void setVelocity(const glm::vec3& velocity);

		/**
		 * Sets the velocity of the rigid body by component. The
		 * velocity is given in world space.
		 * 
		 * @param x The x coordinate of the new velocity of the rigid 
		 * body.
		 * 
		 * @param y The y coordinate of the new velocity of the rigid
		 * body.
		 *
		 * @param z The z coordinate of the new velocity of the rigid
		 * body.
		 */
		void setVelocity(const real x, const real y, const real z);

		/**
		 * Fills the given vector with the velocity of the rigid body.
		 *
		 * @param velocity A pointer to a vector into which to write
		 * the velocity. The velocity is given in world local space.
		 */
		void getVelocity(glm::vec3* velocity) const;

		/**
		 * Gets the velocity of the rigid body.
		 *
		 * @return The velocity of the rigid body. The velocity is 
		 * given in world local space.
		 */
		glm::vec3 getVelocity() const;

		/**
		 * Applies the given change in velocity.
		 */
		void addVelocity(const glm::vec3& deltaVelocity);

		/**
		 * Sets the rotation of the rigid body.
		 *
		 * @param rotation The new rotation of the rigid body. The
		 * rotation is given in world space.
		 */
		void setRotation(const glm::vec3& rotation);

		/**
		 * Sets the rotation of the rigid body by component. The
		 * rotation is the given in the world space.
		 * 
		 * @param x The x coordinate of the new rotation of the rigid 
		 * body.
		 *
		 * @param y The y coordinate of the new rotation of the rigid 
		 * body.
		 *
		 * @param z The z coordinate of the new rotation of the rigid 
		 * body.
		 */
		void setRotation(const real x, const real y, const real z);

		/**
		 * Fills the given vector with the rotation of the rigid body.
		 *
		 * @param rotation A pointer to a vector into which to write 
		 * the rotation. The rotation is given in world local space.
		 */
		void getRotation(glm::vec3* rotation) const;

		/**
		 * Gets the rotation of the rigid body.
		 *
		 * @return The rotation of the rigid body. The rotation is
		 * given in world local space.
		 */
		glm::vec3 getRotation() const;

		/**
		 * Applies the given change in rotation.
		 */
		void addRotation(const glm::vec3& deltaRotation);

		/**
		 * Returns true if the body is awake and responding to
		 * integration.
		 *
		 * @return The awake state of the body.
		 */
		bool getAwake() const
		{
			return isAwake;
		}

		/**
		 * Sets the awake state of the body. If the body is set to be
		 * now awake, then its velocities are also cancelled, since
		 * a moving body that is not awake can cause problems in the
		 * simulation.
		 *
		 * @param awake The new awake state of the body.
		 */
		void setAwake(const bool awake = true);

		/**
		 * Returns true if the body is allowed to go to sleep at
		 * any time.
		 */
		bool getCanSleep() const
		{
			return canSleep;
		}

		/**
		 * Sets whether the body is ever allowed to go to sleep. Bodies
		 * under the player's control, or for which the set of
		 * transient forces applied each frame are not predictable,
		 * should be kept awake.
		 *
		 * @param canSleep Whether the body can now be put to sleep.
		 */
		void setCanSleep(const bool canSleep = true);

		
		/**
		 * @name Force, Torque and Acceleration Set-up Functions
		 *
		 * These functions set up forces and torques to apply to the
		 * rigid body.
		 */

		/**
		 * Clears the forces and torques in the accumulators. This will
		 * be called automatically after each integration step.
		 */
		void clearAccumulators();

		/**
		 * Adds the given force to centre of mass of the rigid body.
		 * The force is expressed in world-coordinates.
		 *
		 * @param force the force to apply.
		 */
		void addForce(const glm::vec3& force);

		/**
		 * Adds the given force to the given point on the rigid body.
		 * Both the force and the
		 * application point are given in world space. Because the
		 * force is not applied at the centre of mass, it may be split
		 * into both a force and torque.
		 *
		 * @param force The force to apply.
		 *
		 * @param point The location at which to apply the force, in
		 * world-coordinates.
		 */
		void addForceAtPoint(const glm::vec3& force, const glm::vec3& point);

		/**
		 * Adds the given force to the given point on the rigid body.
		 * The direction of the force is given in world coordinates,
		 * but the application point is given in body space. This is
		 * uesful for spring forces, or other forces fixed to the
		 * body.
		 *
		 * @param force The force to apply.
		 *
		 * @param point The location at which to apply the force, in
		 * body-coordinates.
		 */
		void addForceAtBodyPoint(const glm::vec3& force, const glm::vec3& point);

		/**
		 * Adds the given torque to the rigid body.
		 * The force is expressed in world-coordinates.
		 *
		 * @param torque The torque to apply.
		 */
		void addTorque(const glm::vec3& torque);

		/**
		 * Sets the constant acceleration of the rigid body.
		 *
		 * @param acceleration The new acceleration of the rigid body.
		 */
		void setAcceleration(const glm::vec3& acceleration);

		/**
		 * Sets the constant acceleration of the rigid body by component
		 *
		 * @param x The x coordinate of the new acceleration of the rigid
		 * body.
		 *
		 * @param y The y coordinate of the new acceleration of the rigid
		 * body.
		 *
		 * @param z The z coordinate of the new acceleration of the rigid
		 * body.
		 */
		void setAcceleration(const real x, const real y, const real z);

		/**
		 * Fills the given vector with the acceleration of the rigid body.
		 *
		 * @param acceleration A pointer to a vector into which to write
		 * the acceleration. The acceleration is given in world local space.
		 */
		void getAcceleration(glm::vec3* acceleration) const;

		/**
		 * Gets the acceleration of the rigid body.
		 *
		 * @return The acceleration of the rigid body. The acceleration is
		 * given in world local space.
		 */
		glm::vec3 getAcceleration() const;

		/**
		 * Fills the given vector with the current accumulated value
		 * for linear acceleration. The acceleration accumulators
		 * are set during the integration step. They can be read to
		 * determine the rigid body's acceleration over the last
		 * integration step. The linear acceleration is given in world
		 * space
		 *
		 * @param linearAcceleration A pointer to a vector to receive
		 * the linear acceleration data
		 */
		void getLastFrameAcceleration(glm::vec3* linearAcceleration) const;

		/**
		 * Gets the current accumulated value for linear acceleration.
		 * The acceleration accumulators are set during 
		 * the integration step. They can be read to determine the 
		 * rigid body's acceleration over the last integration step. 
		 * The linear acceleration is given in world space
		 */
		glm::vec3 getLastFrameAcceleration() const;

		glm::vec3 getLastFramePosition() const;

	protected:
		friend class Contact;
		friend class ContactResolver;
		friend class CGProj::CGContactManager;
		// Hold the list of contacts that involve this body
		// use contacts->nextObject[index];
		int contacts = -1; // -1 == Node_NUll
	};
}

#endif


