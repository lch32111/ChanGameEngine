#include <CGPrecompiled.h>
#include "chanQuatCamera.h"

CG::chanQuatCamera::chanQuatCamera(glm::vec3 position, glm::vec3 up)
	:
	MovementSpeed(QUAT_SPEED),
	MouseSensitivity(QUAT_SENSITIVITY),
	Zoom(QUAT_ZOOM)
{
	Position = position;
	Orientation = glm::quat(0, 0, 0, -1);
	RightAngle = 0.f;
	UpAngle = 0.f;
	Front = glm::vec3(0.0);
	Right = glm::vec3(0.0);
	updateCameraVectors();
}

CG::chanQuatCamera::chanQuatCamera(float posX, float posY, float posZ)
	: 
	MovementSpeed(QUAT_SPEED), 
	MouseSensitivity(QUAT_SENSITIVITY), 
	Zoom(QUAT_ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	Orientation = glm::quat(0, 0, 0, -1);
	RightAngle = 0.f;
	UpAngle = 0.f;
	Front = glm::vec3(0.0);
	Right = glm::vec3(0.0);
	updateCameraVectors();
}

glm::mat4 CG::chanQuatCamera::GetViewMatrix()
{
	// You should know the camera move reversely relative to the user input.
	// That's the point of Graphics Camera!
	glm::mat4 View = glm::mat4_cast(glm::conjugate(Orientation));
	View[3][0] = -(View[0][0] * Position.x + View[1][0] * Position.y + View[2][0] * Position.z);
	View[3][1] = -(View[0][1] * Position.x + View[1][1] * Position.y + View[2][1] * Position.z);
	View[3][2] = -(View[0][2] * Position.x + View[1][2] * Position.y + View[2][2] * Position.z);

	return View;
}

void CG::chanQuatCamera::GetViewMatrix(glm::mat4 & View)
{
	// You should know the camera move reversely relative to the user input.
	// That's the point of Graphics Camera!
	View = glm::mat4_cast(glm::conjugate(Orientation));
	View[3][0] = -(View[0][0] * Position.x + View[1][0] * Position.y + View[2][0] * Position.z);
	View[3][1] = -(View[0][1] * Position.x + View[1][1] * Position.y + View[2][1] * Position.z);
	View[3][2] = -(View[0][2] * Position.x + View[1][2] * Position.y + View[2][2] * Position.z);
}

void CG::chanQuatCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
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

void CG::chanQuatCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	RightAngle += xoffset;
	UpAngle += yoffset;

	updateCameraVectors();
}

void CG::chanQuatCamera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.f && Zoom <= 45.f)
		Zoom -= yoffset;

	if (Zoom <= 1.f)
		Zoom = 1.f;

	if (Zoom >= 45.f)
		Zoom = 45.f;
}

void CG::chanQuatCamera::updateCameraVectors()
{
	// Yaw
	glm::quat aroundY = glm::angleAxis(glm::radians(-RightAngle), glm::vec3(0, 1, 0));

	// Pitch
	glm::quat aroundX = glm::angleAxis(glm::radians(UpAngle), glm::vec3(1, 0, 0));

	// Construct Quaternion Orientation
	Orientation = aroundY * aroundX;

	// Prepare Front and Right vector of eye space
	glm::quat qF = Orientation * glm::quat(0, 0, 0, -1) * glm::conjugate(Orientation);
	Front = { qF.x, qF.y, qF.z };
	Right = glm::cross(Front, glm::vec3(0, 1, 0));
}