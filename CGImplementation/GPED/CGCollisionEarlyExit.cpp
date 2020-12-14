#include <CGPrecompiled.h>
#include <GPED/CGCollisionEarlyExit.h>
#include <GPED/CGCollisionUtil.h>

bool CG::CGCollisionEarlyExit::sphereAndHalfSpace(const CGCollisionSphere & sphere, const CGCollisionPlane & plane)
{
	// Find the distance from the origin
	GPED::real ballDistance = glm::dot(plane.direction, sphere.getAxis(3)) - sphere.radius;

	// Check for the intersection
	return ballDistance <= plane.offset;
}

bool CG::CGCollisionEarlyExit::sphereAndSphere(const CGCollisionSphere & one, const CGCollisionSphere & two)
{
	// Find the vector between the objects
	glm::vec3 midline = one.getAxis(3) - two.getAxis(3);

	// See if it is large enough
	return glm::dot(midline, midline) < (one.radius + two.radius) * (one.radius + two.radius);
}

// This preprocessor definition is only used as a convenience
// in the boxAndBox intersection method
#define TEST_OVERLAP(axis) overlapOnAxis(one, two, (axis), toCentre)
bool CG::CGCollisionEarlyExit::OBBAndOBB(const CGCollisionOBB & one, const CGCollisionOBB & two)
{
	// Find the vector between the two centres
	glm::vec3 toCentre = two.getAxis(3) - one.getAxis(3);

	return
		(
			// Check on box one's axes first
			TEST_OVERLAP(one.getAxis(0)) &&
			TEST_OVERLAP(one.getAxis(1)) &&
			TEST_OVERLAP(one.getAxis(2)) &&

			// And on two's
			TEST_OVERLAP(two.getAxis(0)) &&
			TEST_OVERLAP(two.getAxis(1)) &&
			TEST_OVERLAP(two.getAxis(2)) &&

			// Now on the cross products
			TEST_OVERLAP(glm::cross(one.getAxis(0), two.getAxis(0))) &&
			TEST_OVERLAP(glm::cross(one.getAxis(0), two.getAxis(1))) &&
			TEST_OVERLAP(glm::cross(one.getAxis(0), two.getAxis(2))) &&
			TEST_OVERLAP(glm::cross(one.getAxis(1), two.getAxis(0))) &&
			TEST_OVERLAP(glm::cross(one.getAxis(1), two.getAxis(1))) &&
			TEST_OVERLAP(glm::cross(one.getAxis(1), two.getAxis(2))) &&
			TEST_OVERLAP(glm::cross(one.getAxis(2), two.getAxis(0))) &&
			TEST_OVERLAP(glm::cross(one.getAxis(2), two.getAxis(1))) &&
			TEST_OVERLAP(glm::cross(one.getAxis(2), two.getAxis(2)))
			);
}
#undef TEST_OVERLAP

bool CG::CGCollisionEarlyExit::OBBAndHalfSpace(const CGCollisionOBB & box, const CGCollisionPlane & plane)
{
	// Work out the projected radius of the box onto the plane direction
	GPED::real projectedRadius = transformToAxis(box, plane.direction);

	// Work out how far the box is from the origin
	GPED::real boxDistance = glm::dot(plane.direction, box.getAxis(3)) - plane.offset;

	// Check for the intersection
	return real_abs(boxDistance) <= projectedRadius;
}

// This Code is from BulletPhysics : conservative test for overlap between triangle and aabb
// The process is like making the aabb from triangle points and then do the aabb/aabb test
bool CG::CGCollisionEarlyExit::TriangleAndAABB(const CGCollisionTriangle& triangle, const GPED::c3AABB & aabb)
{
	const glm::vec3 &p1 = triangle.m_points[0];
	const glm::vec3 &p2 = triangle.m_points[1];
	const glm::vec3 &p3 = triangle.m_points[2];

	if (GPED::rMin(GPED::rMin(p1[0], p2[0]), p3[0]) > aabb.max[0]) return false;
	if (GPED::rMax(GPED::rMax(p1[0], p2[0]), p3[0]) < aabb.min[0]) return false;

	if (GPED::rMin(GPED::rMin(p1[2], p2[2]), p3[2]) > aabb.max[2]) return false;
	if (GPED::rMax(GPED::rMax(p1[2], p2[2]), p3[2]) < aabb.min[2]) return false;

	if (GPED::rMin(GPED::rMin(p1[1], p2[1]), p3[1]) > aabb.max[1]) return false;
	if (GPED::rMax(GPED::rMax(p1[1], p2[1]), p3[1]) < aabb.min[1]) return false;
	return true;
}
