#include <CGPrecompiled.h>
#include "GPED_Precision.h"
#include <GPED/CGCollisionPrimitive.h>
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

real GPED::rMin(real a, real b, real c)
{
	GPED::real min = (a < b ? a : b);
	return (min < c? min : c);
}

real GPED::rMax(real a, real b)
{
	return a > b ? a : b;
}

real GPED::rMax(real a, real b, real c)
{
	GPED::real max = (a > b ? a : b);
	return (max > c ? max : c);
}

glm::vec3 GPED::rMin(const glm::vec3 & a, const glm::vec3 & b)
{
	return glm::vec3(rMin(a.x, b.x), rMin(a.y, b.y), rMin(a.z, b.z));
}

glm::vec3 GPED::rMax(const glm::vec3 & a, const glm::vec3 & b)
{
	return glm::vec3(rMax(a.x, b.x), rMax(a.y, b.y), rMax(a.z, b.z));
}

bool GPED::aabbOverlap(const GPED::c3AABB & a, const c3AABB & b)
{
	// Exit with no intersection if separated along an axis
	if (a.max[0] < b.min[0] || a.min[0] > b.max[0]) return false;
	if (a.max[1] < b.min[1] || a.min[1] > b.max[1]) return false;
	if (a.max[2] < b.min[2] || a.min[2] > b.max[2]) return false;

	// Overlapping on all axes means AABBs are intersecting
	return true;
}

// RTCD p180 ~ 181.
bool GPED::rayaabbOverlap(const GPED::c3AABB & a, const GPED::c3RayInput & ray)
{
	GPED::real tmin = -REAL_MAX;
	GPED::real tmax = REAL_MAX;
	// For all three slabs
	for (int i = 0; i < 3; ++i)
	{
		if (real_abs(ray.direction[i]) < REAL_EPSILON)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (ray.startPoint[i] < a.min[i] || ray.startPoint[i] > a.max[i]) return false;
		}
		else
		{
			// Compute intersection t value of ray with near and far plane of slabe
			GPED::real ood = GPED::real(1.0) / ray.direction[i];
			GPED::real t1 = (a.min[i] - ray.startPoint[i]) * ood;
			GPED::real t2 = (a.max[i] - ray.startPoint[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) GPED::Swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			tmin = GPED::rMax(tmin, t1);
			tmax = GPED::rMin(tmax, t2);
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return false;
		}
	}
	// Ray intersects all 3slabs. Return point (q) and intersection t value (tmin)
	// return ray.startPoint + ray.direction * tmin;
	return true;
}

bool GPED::rayaabbIntersection(GPED::c3RayOutput & output, const GPED::c3RayInput & input, const GPED::c3AABB & aaabb)
{
	GPED::real tmin = -REAL_MAX;
	GPED::real tmax = REAL_MAX;
	// For all three slabs
	for (int i = 0; i < 3; ++i)
	{
		if (real_abs(input.direction[i]) < REAL_EPSILON)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (input.startPoint[i] < aaabb.min[i] || input.startPoint[i] > aaabb.max[i]) return false;
		}
		else
		{
			// Compute intersection t value of ray with near and far plane of slabe
			GPED::real ood = GPED::real(1.0) / input.direction[i];
			GPED::real t1 = (aaabb.min[i] - input.startPoint[i]) * ood;
			GPED::real t2 = (aaabb.max[i] - input.startPoint[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) GPED::Swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			tmin = GPED::rMax(tmin, t1);
			tmax = GPED::rMin(tmax, t2);
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return false;
		}
	}
	// Ray intersects all 3slabs. Return point (q) and intersection t value (tmin)
	output.startPoint = input.startPoint;
	output.t = tmin;
	output.hitPoint = input.startPoint + input.direction * tmin;
	return true;
}

void GPED::convertFromCollisionPrimitive(const CG::CGCollisionPrimitive & primitive, c3AABB& outAABB)
{
	primitive.getAABB(outAABB);
}

GPED::c3AABB GPED::makeAABB(const glm::vec3 & position, const glm::vec3 & halfExtents)
{
	c3AABB aabb;

	aabb.min = position - halfExtents;
	aabb.max = position + halfExtents;

	return aabb;
}

GPED::c3AABB GPED::makeAABB(const glm::mat3 & orientation, const glm::vec3 & position, const glm::vec3 & halfExtents)
{
	c3AABB aabb;
	aabb.min = glm::vec3(REAL_MAX);
	aabb.max = glm::vec3(-REAL_MAX);

	glm::vec3 v[8] =
	{
		glm::vec3(-halfExtents.x,-halfExtents.y, -halfExtents.z),
		glm::vec3(-halfExtents.x,-halfExtents.y, halfExtents.z),
		glm::vec3(halfExtents.x, -halfExtents.y, halfExtents.z),
		glm::vec3(halfExtents.x,-halfExtents.y, -halfExtents.z),
		glm::vec3(-halfExtents.x, halfExtents.y, -halfExtents.z),
		glm::vec3(-halfExtents.x, halfExtents.y, halfExtents.z),
		glm::vec3(halfExtents.x, halfExtents.y, halfExtents.z),
		glm::vec3(halfExtents.x, halfExtents.y, -halfExtents.z)
	};

	for (int i = 0; i < 8; ++i)
	{
		v[i] = position + orientation * v[i];

		aabb.min = GPED::rMin(aabb.min, v[i]);
		aabb.max = GPED::rMax(aabb.max, v[i]);
	}

	return aabb;
}

GPED::c3AABB GPED::makeAABB(const glm::vec3 & position, const GPED::real radius)
{
	c3AABB aabb;
	aabb.min = position - glm::vec3(radius);
	aabb.max = position + glm::vec3(radius);
	return aabb;
}
