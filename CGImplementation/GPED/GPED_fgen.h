#ifndef __GPED_FGEN_H__
#define __GPED_FGEN_H__

#include <GPED/GPED_body.h>
#include <GPED/GPED_Pfgen.h>
#include <vector>

namespace GPED
{
	/**
	 * A force generator can be asked to add a force to one or more
	 * bodies.
	 */
	class ForceGenerator
	{
	public:
		/**
		 * Overload this in implementations of the interface to calculate
		 * and update the force applied to the given rigid body
		 */
		virtual void updateForce(RigidBody* body, real duration) = 0;
	};

	/**
	 * A force generator that applies a gravitational force. One instance
	 * can be used for multiple rigid bodies.
	 */
	class Gravity : public ForceGenerator
	{
		/** Holds the acceleration due to gravity. */
		glm::vec3 gravity;

	public:
		/** Creates the generator with the given acceleration. */
		Gravity(const glm::vec3& gravity);

		/** Applies the gravitational force to the given rigid body. */
		virtual void updateForce(RigidBody* body, real duration);
	};

	/**
	 * A force generator that applies a Spring force
	 */
	class Spring : public ForceGenerator
	{
		/**
		 * The point of connection of the spring, in local 
		 * coordinates.
		 */
		glm::vec3 connectionPoint;

		/**
		 * The point of connection of the spring to the other obejct,
		 * in that object's local coordinates.
		 */
		glm::vec3 otherConnectionPoint;

		/** The particle at the other end of the spring. */
		RigidBody* other;

		/** Holds the spring constant. */
		real springConstant;

		/** Holds the rest length of the spring. */
		real restLength;

	public:
		/** Createes a new spring with the given parameters. */
		Spring(const glm::vec3& localConnectionPt,
			RigidBody* other,
			const glm::vec3& otherConnectionPt,
			real springConstant,
			real restLength);

		/** Applies the spring force to the given rigid body. */
		virtual void updateForce(RigidBody* body, real duration);
	};

	/** 
	 * A force generator that applies an aerodynamic force
	 */
	class Aero : public ForceGenerator
	{
	protected:
		/**
		 * Holds the aerodynamic tensor for the surface in body
		 * space.
		 */
		glm::mat3 tensor;

		/**
		 * Holds the relative position of the aerodynamic surface in
		 * body coordinates.
		 */
		glm::vec3 position;

		/**
		 * Holds a pointer to a vector containing the windspeed of
		 * environment. This is easier than managing a separate
		 * windspeed vector per generator and having to update it
		 * manually as the wind changes
		 */
		const glm::vec3* windspeed;

	public:
		/**
		 * Creates a new aerodynamic force generator with the
		 * given properties
		 */
		Aero(const glm::mat3& tensor, const glm::vec3& position, const glm::vec3* windspeed);

		/**
		 * Applies the force to the given rigid body
		 */
		virtual void updateForce(RigidBody* body, real duration);

	protected:
		/**
		 * Uses an explicit tensor matrix to update the force on
		 * the given rigid body. This is exactly the same as for updateForce
		 * only it takes an explicit tensor.
		 */
		void updateForceFromTensor(RigidBody* body, real duration, const glm::mat3& tensor);
	};

	/** 
	 * A force generator with a control aerodynamic surface. This
	 * requires three inertia tensors, for the two extremes and
	 * 'resting' position of the control surface. The latter tensor is
	 * the one inherited from the base class, the two extremes are
	 * defined in this class
	 */
	class AeroControl : public Aero
	{
	protected:
		/**
		 * The aerodynamic tensor for the surface, when the control is at
		 * its maximum value.
		 */
		glm::mat3 maxTensor;

		/**
		 * The aerodynamic tensor for the surface, when the control is at
		 * it minimum value.
		 */
		glm::mat3 minTensor;

		/**
		 * The current position of the control for this surface. This
		 * should range between -1 (in which case the minTensor value
		 * is used), through 0 (where the base-class tensor value is 
		 * used) to  +1 (where the maxTensor value is used).
		 */
		real controlSetting;

	private:
		/**  
		 * Calculates the final aerodynamic tensor for the current
		 * control setting.
		 */
		glm::mat3 getTensor();

	public:
		/**
		 * Creates a new aerodynamic control surface with the given
		 * properties.
		 */
		AeroControl(const glm::mat3& base,
			const glm::mat3& min, const glm::mat3& max,
			const glm::vec3& position, const glm::vec3* windspeed);

		/**
		 * Sets the control position of this control. This should
		 * range in -1 ~ 1. Values outside that range give undefined results.
		 */
		void setControl(real value);

		/**
		 * Applies the force to the given rigid body.
		 */
		virtual void updateForce(RigidBody* body, real duration);
	};

	/** 
	 * A force generator with an aerodynamic surface that can be
	 * re-oriented relative to its rigid body. This derives the
	 */
	class AngledAero : public Aero
	{
		/**
		 * Holds the orientation of the aerodynamic surface relative
		 * to the rigid body to which it is attached.
		 */
		glm::quat orientation;

	public:
		/** 
		 * Creates a new aerodynmaic surface with the given properties
		 */
		AngledAero(const glm::mat3& tensor, const glm::vec3& position, const glm::vec3* windspeed);

		/** 
		 * Sets the relative orientation of the aerodynamic surface,
		 * relative to the rigid body it is attached to. Note that
		 * this doesn't affect the point of connection of the surface
		 * to the body.
		 */
		void setOrientation(const glm::quat& quat);

		/**
		 * Applies the force to the given rigid body.
		 */
		virtual void updateForce(RigidBody* body, real duration);
	};

	/** 
	 * A force generator to apply a buoyant force to a rigid body.
	 */
	class Buoyancy : public ForceGenerator
	{
		/**
		 * The maximum submersion depth of the object before
		 * it generates its maximum buoyancy force.
		 */
		real maxDepth;

		/** 
		 * The volume of the object.
		 */
		real volume;

		/**
		 * The height of the water plane above y = 0. The plane will be
		 * parallel to the XZ plane.
		 */
		real waterHeight;

		/** 
		 * The density of the liquid. Pure water has a density of
		 * 1000kg per cubic meter.
		 */
		real liquidDensity;

		/**
		 * The centre of buoyancy of the rigid body, in body coordinates
		 */
		glm::vec3 centreOfBuoyancy;

	public:
		/** Creates a new buoyancy force with the given parameters. */
		Buoyancy(const glm::vec3& cOfB,
			real maxDepth, real volume, real waterHeight,
			real liquidDensity = 1000.0f);

		/** 
		 * Applies the force to the given rigid body.
		 */
		virtual void updateForce(RigidBody* body, real duration);
	}; 

	/**
	 * Holds all the force generators and the bodies they apply to
	 */
	class ForceRegistry
	{
	protected:
		/**
		 * Keeps track of one force generator and the body it
		 * applies to
		 */
		struct ForceRegistration
		{
			RigidBody* body;
			ForceGenerator* fg;
		};

		/**
		 * Holds the list of registrations.
		 */
		typedef std::vector<ForceRegistration> Registry;
		Registry registrations;

	public:
		/**
		 * Registers the given force generator to apply to the given body.
		 */
		void add(RigidBody* body, ForceGenerator* fg);

		/**
		 * Removes the given registered pair from the registry.
		 * If the pair is not registered, this method will have no effect.
		 */
		void remove(RigidBody* body, ForceGenerator* fg);

		/**
		 * Clears all registrations from the registry. This will
		 * not delete the bodies or the force generators
		 * themselevs, just the records of their connection
		 */
		void clear();

		/**
		 * Calls all the force generators to update the forces of
		 * their corresponding bodies
		 */
		void updateForces(real duration);
	};
}

#endif
