/*
	This code is based on a tutorial code of learnopengl.com.
	I converted this euler camera to quaternion camera.
*/

#ifndef __CHAN_QUAT_CAMERA_H__
#define __CHAN_QUAT_CAMERA_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CGProj
{
	// Default camera values
	const float QUAT_SPEED = 10.0f;
	const float QUAT_SENSITIVITY = 0.1f;
	const float QUAT_ZOOM = 45.0f;

	class chanQuatCamera
	{
	public:
		enum class Camera_Movement
		{
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT
		};

		glm::vec3 Position;
		glm::quat Orientation;
		float RightAngle;
		float UpAngle;

		glm::vec3 Front;
		glm::vec3 Right;

		// Camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;

		chanQuatCamera
		(
			glm::vec3 position = glm::vec3(0.f, 0.f, 0.f),
			glm::vec3 up = glm::vec3(0.f, 1.0f, 0.f)
		);
		chanQuatCamera(float posX, float posY, float posZ);

		glm::mat4 GetViewMatrix();
		void GetViewMatrix(glm::mat4& View);

		void ProcessKeyboard(Camera_Movement direction, float deltaTime);
		void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
		void ProcessMouseScroll(float yoffset);
	private:
		void updateCameraVectors();
	};
}

#endif