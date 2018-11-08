/*
	This is a tutorial code of learnopengl.com.
	I would convert this euler camera to quaternion camera sometime.
*/

#ifndef __CHAN_EULER_CAMERA_H__
#define __CHAN_EULER_CAMERA_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

// Default camera values
const float YAW = 0.0f;
const float PITCH = 0.0f;
const float EULER_SPEED = 2.5f;
const float EULER_SENSITIVITY = 0.1f;
const float EULER_ZOOM = 45.0f;

class chanEulerCamera
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
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// Euler Angles
	float Yaw;
	float Pitch;

	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	chanEulerCamera
	(
		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f),
		glm::vec3 up = glm::vec3(0.f, 1.0f, 0.f),
		float yaw = YAW,
		float pitch = PITCH
	);
	chanEulerCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

private:
	void updateCameraVectors();
};

#endif