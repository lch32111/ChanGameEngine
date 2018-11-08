#ifndef __GPED_PFGEN_H__
#define __GPED_PFGEN_H__

/**
 * A force generator can be asked to add a force to one or more
 * particles.
 */

#include <GPED/GPED_Particle.h>
#include <vector>

namespace GPED
{
	class ParticleForceGenerator
	{
	public:
		/**
		 * Overload this in implementations of the interface to calculate 
		 * and update the force applied to the given particle.
		 */
		virtual void updateForce(GPEDParticle* particle, real duration) = 0;
	};

	/**
	 * A force generator that applies a gravitational force. One instance
	 * can be used for multiple particles.
	 */
	class ParticleGravity : public ParticleForceGenerator
	{
		/** Holds the acceleration due to gravity. */
		glm::vec3 gravity;
	
	public:
		/** Creates the generator with the given acceleration. */
		ParticleGravity(const glm::vec3& gravity);

		/** Applies the gravitational force to the given particle. */
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/**
	 * A force generator that applies a drag force. One instance
	 * can be used for multipple particles
	 */
	class ParticleDrag : public ParticleForceGenerator
	{
		/** Holds the velocity drag coefficient. */
		real k1;

		/** Holds the velocity squared drag coefficient */
		real k2;
	
	public:
		/** Creates the generator with the given coefficinets.*/
		ParticleDrag(real k1, real k2);

		/** Applies the drag force to the given particle*/
		virtual void updateForce(GPEDParticle* particle, real duration);
	};


	/**
	 * A force generator that applies a spring force
	 */
	class ParticleSpring : public ParticleForceGenerator
	{
		/** The Particle at the other end of the spring. */
		GPEDParticle* other;

		/** Holds the spring constant. */
		real springConstant;

		/** Holds the rest length of the spring. */
		real restLength;

	public:
		/** Creates a new spring with the given parameters. */
		ParticleSpring(GPEDParticle* other, real springConstant, real restLength);

		/** Applies the spring force to the given particle.*/
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/**
	 * A force generator that applies a spring force, where
	 * one end is attached to a fixed point in space.
	 */
	class ParticleAnchoredSpring : public ParticleForceGenerator
	{
	protected:
		/** The location of the anchored end of the spring*/
		glm::vec3* anchor;

		/** Holds the spring constant. */
		real springConstant;

		/** Holds the rest length of the spring. */
		real restLength;

	public:
		/** Creates a new spring with the given parameters. */
		ParticleAnchoredSpring(glm::vec3* anchor, real springConstant, real restLength);

		/** Applies the spring force to the given particle*/
		virtual void updateForce(GPEDParticle* particle, real duration);

		/** Retrieve the anchor point. */
		const glm::vec3* getAnchor() const { return anchor; }
	};

	/**
	 * A force generator that applies a spring force only
	 * when extended.
	 */
	class ParticleBungee : public ParticleForceGenerator
	{
		/** The particle at the other end of the spring.*/
		GPEDParticle* other;

		/** Holds the spring constant.*/
		real springConstant;

		/** 
		 * Holds the length of the bungee at the point it begins to 
		 * generate a force
		 */
		real restLength;

	public:
		/** Creates a new bungee with the given parameters */
		ParticleBungee(GPEDParticle* other, real springConstant, real restLength);

		/** Applies the spring force to the given particle*/
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/**
	 * A force generator that applies a bungee force, where
	 * one end is attached to a fixed point in space
	 */
	class ParticleAnchoredBungee : public ParticleAnchoredSpring
	{
	public:
		/** Use the initializer of the ParticleAnchoredSpring. Added by Chan */
		using ParticleAnchoredSpring::ParticleAnchoredSpring;

		/** Applies the spring force to the given particle*/
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/**
	 * A force generator that applies a buoyancy force for a plane of 
	 * liquid parallel to XZ plane.
	 */
	class ParticleBuoyancy : public ParticleForceGenerator
	{
		/**
		 * Consider a particle as a sphere, and it has a radius
		 * With this variable, the submerged part of a particle is
		 * determined.
		 */
		real particleRadius;


		/**
		 * The height of the water plane above y = 0. The plane will be
		 * parallel to the XZ plane.
		 */
		real waterHeight;

		/**
		 * The linear Drag of Water
		 */
		real linearDrag;

		/**
		 * The density of the liquid.Pure water has a density of
		 * 1000 kg per cubic meter.
		 */
		real liquidDensity;

	public:
		/** Creates a new buoyancy force with the given parameters */
		ParticleBuoyancy(real radius, real linearDrag, real waterHeight, real liquidDensity = 1000.0f);

		/** Applies the buoyancy force to the given particle.*/
		virtual void updateForce(GPEDParticle* particle, real duration);
	};

	/**
	 * A force generator that fakes a stiff spring force, and where
	 * one end is attached to a fixed point in space.
	 */
	class ParticleFakeSpring : public ParticleForceGenerator
	{
		/** The location of the anchored and of the spring. */
		glm::vec3* anchor;

		/** Holds the spring constant. */
		real springConstant;

		/** Holds the damping on the oscillation of the spring. */
		real damping;

	public:
		/** Creates a new spring with the given parameters. */
		ParticleFakeSpring(glm::vec3* anchor, real springConstant, real damping);

		/** Apllies the spring force to the given particle. */
		virtual void updateForce(GPEDParticle* particle, real duration);
	};


	/**
	 * Holds all the force generators and the particles they apply to.
	 */
	class ParticleForceRegistry
	{
	protected:
		/**
		 * Keeps track of one force generator and the particle it
		 * applies to
		 */
		struct ParticleForceRegistration
		{
			GPEDParticle* particle;
			ParticleForceGenerator* fg;
		};

		/**
		 * Holds the list of registrations.
		 */
		typedef std::vector<ParticleForceRegistration> Registry;
		Registry registrations;

	public:
		/**
		 * Registers the given force generator to apply to the 
		 * given particle
		 */
		void add(GPEDParticle* particle, ParticleForceGenerator* fg);

		/**
		 * Removes the given registered pair from the registry.
		 * If the pair is not registered, this method will have
		 * no effect.
		 */
		void remove(GPEDParticle* particle, ParticleForceGenerator* fg);

		/**
		 * Clears all registrations from the registry. This will
		 * not delete the particles or the force generators
		 * themselves, just the records of their connections.
		 */
		void clear();

		/**
		 * Calls all the force generators to update the forces of
		 * their corresponding particles.
		 */
		void updateForces(real duration);
	};
}



#endif
