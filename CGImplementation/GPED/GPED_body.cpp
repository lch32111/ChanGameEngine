#include <CGPrecompiled.h>
#include "GPED_body.h"
using namespace GPED;
/**
 * Inline function that creates a transform matrix from a position
 * and orientation.
 */

/*
 * ---------------------------------
 * INTERNAL OR HELPER FUNCTIONS
 * ---------------------------------
 */

/**
 * Internal function that checks the validity of an inverse inertia tensor.
 */
static inline void _checkInverseInertiaTensor(const glm::mat3& iitWorld)
{
	// TODO: Perform a validity check in an assert.
}

/**
 * Internal function to do an inertia tensor transform by a quaternion.
 * Note that the implementation of this function was created by an 
 * automated code-generator and optimizer.
 */
static inline void _transformInertiaTensor(glm::mat3& iitWorld,
	const glm::quat& q, const glm::mat3& iitBody, const glm::mat4& rotmat)
{
	glm::mat3 rot = glm::mat3(rotmat);
	iitWorld = rot * iitBody * glm::transpose(rot); 
}

static inline void _calculateTransformMatrix(glm::mat4& transformMatrix,
	const glm::vec3& position, const glm::quat& orientation)
{
	transformMatrix = glm::mat4(1.0);
	const glm::quat& q = orientation;

	real qxx(q.x * q.x);
	real qyy(q.y * q.y);
	real qzz(q.z * q.z);
	real qxz(q.x * q.z);
	real qxy(q.x * q.y);
	real qyz(q.y * q.z);
	real qwx(q.w * q.x);
	real qwy(q.w * q.y);
	real qwz(q.w * q.z);

	transformMatrix[0][0] = real(1) - real(2) * (qyy + qzz);
	transformMatrix[0][1] = real(2) * (qxy + qwz);
	transformMatrix[0][2] = real(2) * (qxz - qwy);

	transformMatrix[1][0] = real(2) * (qxy - qwz);
	transformMatrix[1][1] = real(1) - real(2) * (qxx + qzz);
	transformMatrix[1][2] = real(2) * (qyz + qwx);

	transformMatrix[2][0] = real(2) * (qxz + qwy);
	transformMatrix[2][1] = real(2) * (qyz - qwx);
	transformMatrix[2][2] = real(1) - real(2) * (qxx + qyy);

	transformMatrix[3][0] = position.x;
	transformMatrix[3][1] = position.y;
	transformMatrix[3][2] = position.z;
}



/*
* --------------------------------------------------------------------------
* FUNCTIONS DECLARED IN HEADER:
* --------------------------------------------------------------------------
*/

void GPED::RigidBody::calculateDerivedData()
{
	orientation = glm::normalize(orientation);
	

	// Calculate the transform matrix for the body.
	_calculateTransformMatrix(transformMatrix, position, orientation);

	// Calculate the inertiaTensor in world space
	_transformInertiaTensor(inverseInertiaTensorWorld,
		orientation, inverseInertiaTensor, transformMatrix);
}

void GPED::RigidBody::integrate(real duration)
{
	if (!isAwake) return;
	
	// Calculate linear acceleration from force inputs
	lastFrameAcceleration = acceleration;
	lastFrameAcceleration += forceAccum * inverseMass;

	// Calculate angular acceleration from torque inputs.
	glm::vec3 angularAcceleration = inverseInertiaTensorWorld * torqueAccum;

	// Adjust velocities
	// Update linear velocity from both acceleration and impulse
	velocity += lastFrameAcceleration * duration;

	// Update angular velocity from both acceleration and impulse.
	rotation += angularAcceleration * duration;

	// Impose drag.
	velocity *= real_pow(linearDamping, duration);
	rotation *= real_pow(angularDamping, duration);

	// Adjust positions
	// Update linear position
	lastFramePosition = position;
	position += velocity * duration;

	// Update angular position
	glm::quat deltaOrien(0, rotation * duration);
	deltaOrien *= orientation;
	orientation.w += deltaOrien.w * ((real)0.5);
	orientation.x += deltaOrien.x * ((real)0.5);
	orientation.y += deltaOrien.y * ((real)0.5);
	orientation.z += deltaOrien.z * ((real)0.5);

	// Normalise the orientation, and update the matrices with the new
	// position and orientation
	calculateDerivedData();

	// Clear accumulators.
	clearAccumulators();

	// Update the kinetic energy store, and possibly put the body to
	// sleep
	if (canSleep)
	{
		real currentMotion = glm::dot(velocity, velocity) + glm::dot(rotation, rotation);
		real bias = real_pow(0.5, duration);
		motion = bias * motion + (1 - bias) * currentMotion;

		if (motion < sleepEpsilon) setAwake(false);
		else if (motion > 10 * sleepEpsilon) motion = 10 * sleepEpsilon;
	}
}

void GPED::RigidBody::setMass(const real mass)
{
	assert(mass > real_epsilon);
	RigidBody::inverseMass = (real(1.0)) / mass;
}

real GPED::RigidBody::getMass() const
{
	if (inverseMass == 0)
		return REAL_MAX;
	else
		return (real(1.0)) / inverseMass;
}

void GPED::RigidBody::setInverseMass(const real inverseMass)
{
	RigidBody::inverseMass = inverseMass;
}

real GPED::RigidBody::getInverseMass() const
{
	return inverseMass;
}

bool GPED::RigidBody::hasFiniteMass() const
{
	return inverseMass > real_epsilon;
}

void GPED::RigidBody::setInertiaTensor(const glm::mat3& inertiaTensor)
{
	inverseInertiaTensor = glm::inverse(inertiaTensor);
	_checkInverseInertiaTensor(inverseInertiaTensor);
}

void GPED::RigidBody::getInertiaTensor(glm::mat3& inertiaTensor) const
{
	inertiaTensor = glm::inverse(inverseInertiaTensor);
}

glm::mat3 GPED::RigidBody::getInertiaTensor() const
{
	return glm::inverse(inverseInertiaTensor);
}

void GPED::RigidBody::getInertiaTensorWorld(glm::mat3* inertiaTensor) const
{
	*inertiaTensor = glm::inverse(inverseInertiaTensorWorld);
}

glm::mat3 GPED::RigidBody::getInertiaTensorWorld() const
{
	return glm::inverse(inverseInertiaTensorWorld);
}

void GPED::RigidBody::setInverseInertiaTensor(const glm::mat3& inverseInertiaTensor)
{
	_checkInverseInertiaTensor(inverseInertiaTensor);
	RigidBody::inverseInertiaTensor = inverseInertiaTensor;
}

void GPED::RigidBody::getInverseInertiaTensor(glm::mat3* inverseInertiaTensor) const
{
	*inverseInertiaTensor = RigidBody::inverseInertiaTensor;
}

glm::mat3 GPED::RigidBody::getInverseInertiaTensor() const
{
	return inverseInertiaTensor;
}

void GPED::RigidBody::getInverseInertiaTensorWorld(glm::mat3* inverseInertiaTensor) const
{
	*inverseInertiaTensor = inverseInertiaTensorWorld;
}

glm::mat3 GPED::RigidBody::getInverseInertiaTensorWorld() const
{
	return inverseInertiaTensorWorld;
}

void GPED::RigidBody::setDamping(const real linearDamping, const real angularDamping)
{
	RigidBody::linearDamping = linearDamping;
	RigidBody::angularDamping = angularDamping;
}

void GPED::RigidBody::setLinearDamping(const real linearDamping)
{
	RigidBody::linearDamping = linearDamping;
}

real GPED::RigidBody::getLinearDamping() const
{
	return linearDamping;
}

void GPED::RigidBody::setAngularDamping(const real angularDamping)
{
	RigidBody::angularDamping = angularDamping;
}

real GPED::RigidBody::getAngularDamping() const
{
	return angularDamping;
}

void GPED::RigidBody::setPosition(const glm::vec3 & position)
{
	RigidBody::position = position;
}

void GPED::RigidBody::setPosition(const real x, const real y, const real z)
{
	RigidBody::position = glm::vec3(x, y, z);
}

void GPED::RigidBody::getPosition(glm::vec3* position) const
{
	*position = RigidBody::position;
}

glm::vec3 GPED::RigidBody::getPosition() const
{
	return position;
}

void GPED::RigidBody::setOrientation(const glm::quat & orientation)
{
	// glm also includes constructing valid rotation quaterninon in case of
	// (0,0,0,0)
	RigidBody::orientation = glm::normalize(orientation);
}

void GPED::RigidBody::setOrientation(const real r, const real i, const real j, const real k)
{
	orientation = glm::normalize(glm::quat(r, i, j, k));
}

void GPED::RigidBody::getOrientation(glm::quat* orientation) const
{
	*orientation = RigidBody::orientation;
}

glm::quat GPED::RigidBody::getOrientation() const
{
	return orientation;
}

void GPED::RigidBody::getOrientation(glm::mat3* matrix) const
{
	*matrix = glm::mat3(transformMatrix);
}

void GPED::RigidBody::getTransform(glm::mat4* transform) const
{
	*transform = RigidBody::transformMatrix;
}

glm::mat4 GPED::RigidBody::getTransform() const
{
	return transformMatrix;
}

glm::vec3 GPED::RigidBody::getPointInLocalSpace(const glm::vec3& point) const
{
	glm::vec3 temp = point;

	// Return to the origin.
	temp.x -= transformMatrix[3][0];
	temp.y -= transformMatrix[3][1];
	temp.z -= transformMatrix[3][2];

	// Return to the original axis. 
	// the inverse of a pure rotation matrix is its transpose. (orthgonal matrix)
	// To convert point from world to local
	// you need to inverse(transpose) the worldTransformMatrix
	// it is related to the change of basis.
	return  glm::transpose(glm::mat3(transformMatrix)) * temp;
}

glm::vec3 GPED::RigidBody::getPointInWorldSpace(const glm::vec3 & point) const
{
	return glm::vec3(transformMatrix * glm::vec4(point, 1.0));
}

glm::vec3 GPED::RigidBody::getDirectionInLocalSpace(const glm::vec3 & direction) const
{
	// Return to the original axis. 
	// the inverse of a pure rotation matrix is its transpose.
	// To convert point from world to local, 
	// you need to inverse(transpose) the worldTransformMatrix
	// it is related to the change of basis.
	return glm::transpose(glm::mat3(transformMatrix)) * direction;
}

glm::vec3 GPED::RigidBody::getDirectionInWorldSpace(const glm::vec3 & direction) const
{
	return glm::mat3(transformMatrix) * direction;
}

void GPED::RigidBody::setVelocity(const glm::vec3 & velocity)
{
	RigidBody::velocity = velocity;
}

void GPED::RigidBody::setVelocity(const real x, const real y, const real z)
{
	RigidBody::velocity = glm::vec3(x, y, z);
}

void GPED::RigidBody::getVelocity(glm::vec3 * velocity) const
{
	*velocity = RigidBody::velocity;
}

glm::vec3 GPED::RigidBody::getVelocity() const
{
	return velocity;
}

void GPED::RigidBody::addVelocity(const glm::vec3 & deltaVelocity)
{
	velocity += deltaVelocity;
}

void GPED::RigidBody::setRotation(const glm::vec3 & rotation)
{
	RigidBody::rotation = rotation;
}

void GPED::RigidBody::setRotation(const real x, const real y, const real z)
{
	RigidBody::rotation = glm::vec3(x, y, z);
}

void GPED::RigidBody::getRotation(glm::vec3 * rotation) const
{
	*rotation = RigidBody::rotation;
}

glm::vec3 GPED::RigidBody::getRotation() const
{
	return rotation;
}

void GPED::RigidBody::addRotation(const glm::vec3 & deltaRotation)
{
	rotation += deltaRotation;
}

void GPED::RigidBody::setAwake(const bool awake)
{
	if (awake)
	{
		isAwake = true;

		// Add a bit of motion to avoid it falling asleep immediately.
		motion = sleepEpsilon * real(2.0);
	}
	else
	{
		isAwake = false;
		velocity = glm::vec3(real(0));
		rotation = glm::vec3(real(0));
	}
}

void GPED::RigidBody::setCanSleep(const bool canSleep)
{
	RigidBody::canSleep = canSleep;

	if (!canSleep && !isAwake) setAwake();
}

void GPED::RigidBody::clearAccumulators()
{
	forceAccum = glm::vec3(0.0);
	torqueAccum = glm::vec3(0.0);
}

void GPED::RigidBody::addForce(const glm::vec3& force)
{
	forceAccum += force;
	isAwake = true;
}

void GPED::RigidBody::addForceAtPoint(const glm::vec3& force, const glm::vec3& point)
{
	// Convert to coordinate relative to center of mass.
	glm::vec3 pt = point;
	pt -= position;

	forceAccum += force;
	torqueAccum += glm::cross(pt, force);

	isAwake = true;
}

void GPED::RigidBody::addForceAtBodyPoint(const glm::vec3& force, const glm::vec3& point)
{
	// Convert to coordinates relative to center of mass
	glm::vec3 pt = getPointInWorldSpace(point);
	addForceAtPoint(force, pt);
}

void GPED::RigidBody::addTorque(const glm::vec3& torque)
{
	torqueAccum += torque;
	isAwake = true;
}

void GPED::RigidBody::setAcceleration(const glm::vec3 & acceleration)
{
	RigidBody::acceleration = acceleration;
}

void GPED::RigidBody::setAcceleration(const real x, const real y, const real z)
{
	RigidBody::acceleration = glm::vec3(x, y, z);
}

void GPED::RigidBody::getAcceleration(glm::vec3 * acceleration) const
{
	*acceleration = RigidBody::acceleration;
}

glm::vec3 GPED::RigidBody::getAcceleration() const
{
	return acceleration;
}

void GPED::RigidBody::getLastFrameAcceleration(glm::vec3 * linearAcceleration) const
{
	*linearAcceleration = lastFrameAcceleration;
}

glm::vec3 GPED::RigidBody::getLastFrameAcceleration() const
{
	return lastFrameAcceleration;
}

glm::vec3 GPED::RigidBody::getLastFramePosition() const
{
	return lastFramePosition;
}
