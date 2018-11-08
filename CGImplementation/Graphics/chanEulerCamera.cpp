#include "chanEulerCamera.h"

chanEulerCamera::chanEulerCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	:
	Front(glm::vec3(0.f, 0.f, -1.f)),
	MovementSpeed(EULER_SPEED),
	MouseSensitivity(EULER_SENSITIVITY),
	Zoom(EULER_ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

chanEulerCamera::chanEulerCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: Front(glm::vec3(0.f, 0.f, -1.f)), MovementSpeed(EULER_SPEED), MouseSensitivity(EULER_SENSITIVITY), Zoom(EULER_ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

glm::mat4 chanEulerCamera::GetViewMatrix()
{
	glm::vec3 zaxis = Position - (Position + Front);
	glm::vec3 xaxis = glm::normalize(glm::cross(WorldUp, zaxis));
	glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	glm::mat4 result(1);
	result[0][0] = xaxis.x;
	result[1][0] = xaxis.y;
	result[2][0] = xaxis.z;
	result[0][1] = yaxis.x;
	result[1][1] = yaxis.y;
	result[2][1] = yaxis.z;
	result[0][2] = zaxis.x;
	result[1][2] = zaxis.y;
	result[2][2] = zaxis.z;
	result[3][0] = -glm::dot(xaxis, Position);
	result[3][1] = -glm::dot(yaxis, Position);
	result[3][2] = -glm::dot(zaxis, Position);

	return result;
	// return glm::lookAt(Position, Position + Front, WorldUp);
}

void chanEulerCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;

	if (direction == Camera_Movement::FORWARD)
		Position += Front * velocity;

	if (direction == Camera_Movement::BACKWARD)
		Position -= Front * velocity;

	if (direction == Camera_Movement::LEFT)
		Position -= Right * velocity;

	if (direction == Camera_Movement::RIGHT)
		Position += Right * velocity;
}

void chanEulerCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	updateCameraVectors();
}

void chanEulerCamera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.f && Zoom <= 45.f)
		Zoom -= yoffset;

	if (Zoom <= 1.f)
		Zoom = 1.f;

	if (Zoom >= 45.f)
		Zoom = 45.f;
}

void chanEulerCamera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
	front.y = sin(glm::radians(Pitch));
	front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));

	Front = glm::normalize(front);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}