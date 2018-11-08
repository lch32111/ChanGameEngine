#include "chanQuatCamera.h"

chanQuatCamera::chanQuatCamera(glm::vec3 position, glm::vec3 up)
	:
	MovementSpeed(QUAT_SPEED),
	MouseSensitivity(QUAT_SENSITIVITY),
	Zoom(QUAT_ZOOM)
{
	Position = position;
	Orientation = glm::quat(0, 0, 0, -1);
	RightAngle = 0.f;
	UpAngle = 0.f;
	updateCameraVectors();
}

chanQuatCamera::chanQuatCamera(float posX, float posY, float posZ, float upX, float upY, float upZ)
	: MovementSpeed(QUAT_SPEED), MouseSensitivity(QUAT_SENSITIVITY), Zoom(QUAT_ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	Orientation = glm::quat(0, 0, 0, 1);
	RightAngle = 0.f;
	UpAngle = 0.f;
	updateCameraVectors();
}

glm::mat4 chanQuatCamera::GetViewMatrix()
{
	// You should know the camera move reversely relative to the user input.
	// That's the point of Graphics Camera!
	glm::mat4 View = glm::mat4_cast(glm::conjugate(Orientation));
	View[3][0] = -(View[0][0] * Position.x + View[1][0] * Position.y + View[2][0] * Position.z);
	View[3][1] = -(View[0][1] * Position.x + View[1][1] * Position.y + View[2][1] * Position.z);
	View[3][2] = -(View[0][2] * Position.x + View[1][2] * Position.y + View[2][2] * Position.z);

	return View;
}

void chanQuatCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;

	glm::quat qF = Orientation * glm::quat(0, 0, 0, -1) * glm::conjugate(Orientation);
	glm::vec3 Front = { qF.x, qF.y, qF.z };
	glm::vec3 Right = glm::cross(Front, glm::vec3(0, 1, 0));

	if (direction == Camera_Movement::FORWARD)
		Position += Front * velocity;

	if (direction == Camera_Movement::BACKWARD)
		Position -= Front * velocity;

	if (direction == Camera_Movement::LEFT)
		Position -= Right * velocity;

	if (direction == Camera_Movement::RIGHT)
		Position += Right * velocity;
}

void chanQuatCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	RightAngle += xoffset;
	UpAngle += yoffset;

	updateCameraVectors();
}

void chanQuatCamera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.f && Zoom <= 45.f)
		Zoom -= yoffset;

	if (Zoom <= 1.f)
		Zoom = 1.f;

	if (Zoom >= 45.f)
		Zoom = 45.f;
}

void chanQuatCamera::updateCameraVectors()
{
	// Yaw
	glm::quat aroundY = glm::angleAxis(glm::radians(-RightAngle), glm::vec3(0, 1, 0));

	// Pitch
	glm::quat aroundX = glm::angleAxis(glm::radians(UpAngle), glm::vec3(1, 0, 0));

	Orientation = aroundY * aroundX;
}