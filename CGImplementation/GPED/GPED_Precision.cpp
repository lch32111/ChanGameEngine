#include "GPED_Precision.h"
using namespace GPED;

real GPED::sleepEpsilon = ((real)0.05);

void GPED::setSleepEsilon(real value)
{
	GPED::sleepEpsilon = value;
}

real GPED::getSleepEpsilon()
{
	return GPED::sleepEpsilon;
}

glm::mat3 GPED::getSkewSymmetric(const glm::vec3& v)
{
	glm::mat3 result;

	result[0][0] = result[1][1] = result[2][2] = 0;
	result[0][1] = v.z;
	result[0][2] = -v.y;

	result[1][0] = -v.z;
	result[1][2] = v.x;

	result[2][0] = v.y;
	result[2][1] = -v.x;

	return result;
}

void GPED::getSkewSymmetric(const glm::vec3& v, glm::mat3& mat)
{
	mat[0][0] = mat[1][1] = mat[2][2] = 0;
	mat[0][1] = v.z;
	mat[0][2] = -v.y;

	mat[1][0] = -v.z;
	mat[1][2] = v.x;

	mat[2][0] = v.y;
	mat[2][1] = -v.x;
}


glm::mat3 GPED::getInertiaTensorCoeffs(real ix, real iy, real iz,
	real ixy, real ixz, real iyz)
{
	glm::mat3 result;
	
	result[0][0] = ix;
	result[1][0] = result[0][1] = -ixy;
	result[2][0] = result[0][2] = -ixz;
	result[1][1] = iy;
	result[2][1] = result[1][2] = -iyz;
	result[2][2] = iz;

	return result;
}

void GPED::getInertiaTensorCoeffs(glm::mat3 & mat, 
	real ix, real iy, real iz, real ixy, real ixz, real iyz)
{
	mat[0][0] = ix;
	mat[1][0] = mat[0][1] = -ixy;
	mat[2][0] = mat[0][2] = -ixz;
	mat[1][1] = iy;
	mat[2][1] = mat[1][2] = -iyz;
	mat[2][2] = iz;
}

/**
* Sets the value of the matrix as an inertia tensor of
* a rectangular block aligned with the body's coordinate
* system with the given axis half-sizes and mass
*/
glm::mat3 GPED::getBlockInertiaTensor(const glm::vec3& halfSizes, real mass)
{
	glm::vec3 square = halfSizes * halfSizes;
	return getInertiaTensorCoeffs
	(
		0.3f * mass * (square.y + square.z),
		0.3f * mass * (square.x + square.z),
		0.3f * mass * (square.x + square.y)
	);
}

real GPED::rMin(real a, real b)
{
	return a < b ? a : b;
}

real GPED::rMax(real a, real b)
{
	return a > b ? a : b;
}

glm::vec3 GPED::rMin(const glm::vec3 & a, const glm::vec3 & b)
{
	return glm::vec3(rMin(a.x, b.x), rMin(a.y, b.y), rMin(a.z, b.z));
}

glm::vec3 GPED::rMax(const glm::vec3 & a, const glm::vec3 & b)
{
	return glm::vec3(rMax(a.x, b.x), rMax(a.y, b.y), rMax(a.z, b.z));
}
