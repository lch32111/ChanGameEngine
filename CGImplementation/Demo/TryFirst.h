#pragma once
#ifndef __TRY_FIRST_H__
#define __TRY_FIRST_H__

#include <GLFW/glfw3.h>

#include <Graphics/Shader.h>
#include <Graphics/chanQuatCamera.h>

#include <GPED/GPED_contacts.h>
#include <GPED/GPED_contacts.h>
#include <GPED/GPED_collide_fine.h>
#include <GPED/PS_AmmoRound.h>
#include <GPED/PS_Box.h>
#include <GPED/CGBroadPhase.h>

#include <string>
#include <vector>
#include <Graphics/GLPrimitiveUtil.h>

namespace CGProj
{
	struct BroadRenderer
	{
		const DynamicAABBTree* tree;
		Shader* m_shader;
		glm::mat4* m_projection;
		glm::mat4* m_view;

		void draw(Shader* shader, glm::mat4* proj, glm::mat4* view)
		{
			m_shader = shader;
			m_projection = proj;
			m_view = view;

			shader->use();
			shader->setMat4("projection", *m_projection);
			shader->setMat4("view", *view);
			if(tree->GetHeight() >= 0)
				recursiveDraw(tree->m_root);
		}

		void recursiveDraw(int index)
		{
			if(!tree->m_nodes[index].isLeaf())
				render(tree->m_nodes[index].aabb, glm::vec3(1.0, 1.0, 0), 1.3);
			else
				render(tree->m_nodes[index].aabb, glm::vec3(1.0, 0, 0), 1.5);

			int left = tree->m_nodes[index].left;
			int right = tree->m_nodes[index].right;

			if (left != Node_Null)
				recursiveDraw(left);

			if (right != Node_Null)
				recursiveDraw(right);
		}
		
		void render(const GPED::c3AABB& aabb, const glm::vec3& Color, float lineWidth)
		{
			glm::mat4 model = glm::mat4(1.0);
			glm::vec3 halfextents = (aabb.max - aabb.min) / 2.f;
			glm::vec3 pos = aabb.min + halfextents;
			model = glm::translate(model, pos);
			model = glm::scale(model, halfextents);
			m_shader->setMat4("model", model);
			m_shader->setVec3("Color", Color);
			wireRenderCube(lineWidth);
		}
	};

	class TryFirst
	{
	public:
		void initGraphics();
		void initImgui();
		void deinit();

		void updateImgui();
		void updateSimulation(float deltaTime, float lastFrame);
		void display(int width, int height);

		void key(GLFWwindow* app_window, float deltaTime);
		void mouse(double xpos, double ypos);
		void scroll(double yoffset);


	private:
		/* Imgui Control Parameters*/
		ShotType currentShotType;
		GPED::real contactFriction = 0.9;
		GPED::real contactRestitution = 0.1;
		GPED::real contactTolerance = 0.1;

		Shader simpleShader;
		chanQuatCamera camera;
		float lastX = 400, lastY = 300;
		bool firstMouse = true;
		bool start = false;
		bool Fkey = false;
		bool tabKey = false;

		bool UIControl = false;
		bool GameControl = true;

		CGBroadPhase FirstBroadPhase;
		struct broadResult
		{
			std::vector<std::pair<GPED::CollisionPrimitive*, GPED::CollisionPrimitive*>> vPairs;

			void AddPair(void* proxyUserDataA, void* proxyUserDataB)
			{
				GPED::CollisionPrimitive* aa = (GPED::CollisionPrimitive*)proxyUserDataA;
				GPED::CollisionPrimitive* bb = (GPED::CollisionPrimitive*)proxyUserDataB;

				vPairs.push_back(std::make_pair(aa,bb));
			}
		};
		broadResult firstResult;

		Shader wireShader;
		BroadRenderer bRender;
		
		static const int maxContacts = 1000;
		GPED::Contact contacts[maxContacts];
		GPED::CollisionData cData;
		GPED::ContactResolver resolver;

		const static unsigned ammoRounds = 100;
		AmmoRound ammo[ammoRounds];

		const static unsigned boxes = 70;
		Box boxData[boxes];

		void updateObjects(float duration, float lastFrame);
		void SyncAndUpdate();
		void broadPhase();
		void generateContacts(GPED::CollisionData& cData);

		void fire();
		void totalFire();
		void reset();

		unsigned int woodTexture;
		unsigned int containerTexture;
	};
}

#endif