#ifndef __GPED_FIREWORKS_H__
#define __GPED_FIREWORKS_H__

#include <GPED/GPED_Particle.h>
#include <GPED/GPED_random.h>
#include <Graphics/Shader.h>

static GPED::Random crandom;

namespace GPED
{
	class GPEDFirework : public GPEDParticle
	{
	public:
		unsigned type;
		real age;

		bool update(real duration)
		{
			// update our physical state
			integrate(duration);

			age -= duration;
			return (age < 0 || m_position.y < 0);
		}
	};

	struct FireworkRule
	{
		unsigned type;
		real minAge;
		real maxAge;
		glm::vec3 minVelocity;
		glm::vec3 maxVelocity;
		real damping;

		struct Payload
		{
			unsigned type;
			unsigned count;

			void set(unsigned type, unsigned count)
			{
				Payload::type = type;
				Payload::count = count;
			}
		};
		unsigned payloadCount;
		Payload* payloads;

		FireworkRule() 
			: payloadCount(0), payloads(NULL)
		{ }

		void init(unsigned count)
		{
			FireworkRule::payloadCount = count;
			payloads = new Payload[count];
		}

		~FireworkRule()
		{
			if (payloads != NULL) delete[] payloads;
		}

		void setParameters(unsigned type,
			real minAge, real maxAge,
			const glm::vec3 &minVelocity, const glm::vec3 &maxVelocity,
			real damping)
		{
			FireworkRule::type = type;
			FireworkRule::minAge = minAge;
			FireworkRule::maxAge = maxAge;
			FireworkRule::minVelocity = minVelocity;
			FireworkRule::maxVelocity = maxVelocity;
			FireworkRule::damping = damping;
		}

		void create(GPEDFirework* firework, const GPEDFirework* parent = NULL) const
		{
			firework->type = type;
			firework->age = crandom.randomReal(minAge, maxAge);

			glm::vec3 vel(real(0.0));
			if (parent)
			{
				// The position and velocity are based on the parent
				firework->setPosition(parent->getPosition());
				vel += parent->getVelocity();
			}
			else
			{
				glm::vec3 start(real(0.0));
				int x = (int)crandom.randomInt(3) - 1;
				start.x = real(5.0) * real(x);
				firework->setPosition(start);
			}

			vel += crandom.randomVector(minVelocity, maxVelocity);
			firework->setVelocity(vel);
			firework->setMass(1);
			firework->setDamping(damping);
			firework->setAcceleration(glm::vec3(0, -10, 0)); // gravity
			firework->clearAccumulator();
		}
	};

	class FireworksDemo
	{
		const static unsigned maxFireworks = 1024;
		GPEDFirework fireworks[maxFireworks];
		unsigned nextFirework;

		const static unsigned ruleCount = 9;
		FireworkRule rules[ruleCount];
		void initFireworkRules();
		void create(unsigned type, unsigned number, const GPEDFirework* parent);
		void renderQuad();
		unsigned int quadVAO = 0;
		unsigned int quadVBO;
	public:
		FireworksDemo();
		~FireworksDemo();
		void create(unsigned type, const GPEDFirework* parent);
		void update(float duration);
		void render(Shader* shader, glm::mat4& view, glm::mat4& proj);
	};
}

#endif