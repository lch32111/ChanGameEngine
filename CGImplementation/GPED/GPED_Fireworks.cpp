#include "GPED_Fireworks.h"
#include <glad/glad.h>
using namespace GPED;

void FireworksDemo::initFireworkRules()
{
	// Go through the firework types and create their rules.
	rules[0].init(2);
	rules[0].setParameters(
		1, // type
		0.5f, 1.4f, // age range
		glm::vec3(-5, 25, -5), // min velocity
		glm::vec3(5, 28, 5), // max velocity
		0.1f // damping
	);
	rules[0].payloads[0].set(3, 5);
	rules[0].payloads[1].set(5, 5);

	rules[1].init(1);
	rules[1].setParameters(
		2, // type
		0.5f, 1.0f, // age range
		glm::vec3(-5, 10, -5), // min velocity
		glm::vec3(5, 20, 5), // max velocity
		0.8f // damping
	);
	rules[1].payloads[0].set(4, 2);

	rules[2].init(0);
	rules[2].setParameters(
		3, // type
		0.5f, 1.5f, // age range
		glm::vec3(-5, -5, -5), // min velocity
		glm::vec3(5, 5, 5), // max velocity
		0.1f // damping
	);

	rules[3].init(0);
	rules[3].setParameters(
		4, // type
		0.25f, 0.5f, // age range
		glm::vec3(-20, 5, -5), // min velocity
		glm::vec3(20, 5, 5), // max velocity
		0.2f // damping
	);

	rules[4].init(1);
	rules[4].setParameters(
		5, // type
		0.5f, 1.0f, // age range
		glm::vec3(-20, 2, -5), // min velocity
		glm::vec3(20, 18, 5), // max velocity
		0.01f // damping
	);
	rules[4].payloads[0].set(3, 5);

	rules[5].init(0);
	rules[5].setParameters(
		6, // type
		3, 5, // age range
		glm::vec3(-5, 5, -5), // min velocity
		glm::vec3(5, 10, 5), // max velocity
		0.95f // damping
	);

	rules[6].init(1);
	rules[6].setParameters(
		7, // type
		4, 5, // age range
		glm::vec3(-5, 50, -5), // min velocity
		glm::vec3(5, 60, 5), // max velocity
		0.01f // damping
	);
	rules[6].payloads[0].set(8, 10);

	rules[7].init(0);
	rules[7].setParameters(
		8, // type
		0.25f, 0.5f, // age range
		glm::vec3(-1, -1, -1), // min velocity
		glm::vec3(1, 1, 1), // max velocity
		0.01f // damping
	);

	rules[8].init(0);
	rules[8].setParameters(
		9, // type
		3, 5, // age range
		glm::vec3(-15, 10, -5), // min velocity
		glm::vec3(15, 15, 5), // max velocity
		0.95f // damping
	);
	// ... and so on for other firework types ...
}

void GPED::FireworksDemo::create(unsigned type, unsigned number, const GPEDFirework * parent)
{
	for (unsigned i = 0; i < number; ++i)
		create(type, parent);
}

void GPED::FireworksDemo::create(unsigned type, const GPEDFirework * parent)
{
	// Get the rule needed to create this firework.
	FireworkRule* rule = rules + (type - 1);

	// Create the firework.
	rule->create(fireworks + nextFirework, parent);

	// Increment the index for the next firework
	nextFirework = (nextFirework + 1) % maxFireworks;
}

GPED::FireworksDemo::FireworksDemo()
	:nextFirework(0)
{
	for (GPEDFirework* firework = fireworks; firework < fireworks + maxFireworks; ++firework)
	{
		firework->type = 0;
	}

	initFireworkRules();
}

GPED::FireworksDemo::~FireworksDemo()
{

}

void GPED::FireworksDemo::update(float duration)
{
	// Find the duration of the last frame in seconds
	for (GPEDFirework* firework = fireworks; firework < fireworks + maxFireworks; ++firework)
	{
		// Check if we need to process this firework
		if (firework->type > 0)
		{
			// Does it need removing?
			if (firework->update(duration))
			{
				// Find the appropriate rule
				FireworkRule* rule = rules + (firework->type - 1);

				// Delete the current firework (this doens't affect its
				// position and velocity for passing to the create function
				// just whether or not it is processed for rendering or
				// physics
				firework->type = 0;

				// Add the payload
				for (unsigned i = 0; i < rule->payloadCount; ++i)
				{
					FireworkRule::Payload* payload = rule->payloads + i;
					create(payload->type, payload->count, firework);
				}
			}
		}
	}
}

void GPED::FireworksDemo::render(CGProj::Shader * shader, glm::mat4& view, glm::mat4& proj)
{
	shader->use();
	shader->setMat4("view", view);
	shader->setMat4("projection", proj);

	glm::mat4 model(1.0);
	for (GPEDFirework* firework = fireworks; firework < fireworks + maxFireworks; ++firework)
	{
		if (firework->type > 0)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, firework->getPosition());
			model = glm::scale(model, glm::vec3(0.2f));
			shader->setMat4("model", model);

			switch (firework->type)
			{
			case 1: shader->setVec3("particleColor", glm::vec3(0.5f, 0, 0)); break;
			case 2: shader->setVec3("particleColor", glm::vec3(0.65f, 0.32f, 0.71f)); break;
			case 3: shader->setVec3("particleColor", glm::vec3(0.1868f, 0.32f, 0.68f)); break;
			case 4: shader->setVec3("particleColor", glm::vec3(0.08f, 0.698f, 0.1695f)); break;
			case 5: shader->setVec3("particleColor", glm::vec3(0.42f, 0.77f, 0.348f)); break;
			case 6: shader->setVec3("particleColor", glm::vec3(0.42f, 0.88f, 0.92f)); break;
			case 7: shader->setVec3("particleColor", glm::vec3(0.63f, 0.21f, 0.1f)); break;
			case 8: shader->setVec3("particleColor", glm::vec3(0.0f, 0.58f, 0.42f)); break;
			case 9: shader->setVec3("particleColor", glm::vec3(0.98f, 0.88f, 0.f)); break;
			}

			
			renderQuad();
		}
	}
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------

void GPED::FireworksDemo::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}