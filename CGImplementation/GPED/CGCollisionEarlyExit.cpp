#include <GPED/CGCollisionEarlyExit.h>
#include <GPED/CGCollisionUtil.h>

bool CGProj::CGCollisionEarlyExit::sphereAndHalfSpace(const CGCollisionSphere & sphere, const CGCollisionPlane & plane)
{
	// Find the distance from the origin
	GPED::real ballDistance = glm::dot(plane.direction, sphere.getAxis(3)) - sphere.radius;

	// Check for the intersection
	return ballDistance <= plane.offset;
}

bool CGProj::CGCollisionEarlyExit::sphereAndSphere(const CGCollisionSphere & one, const CGCollisionSphere & two)
{
	// Find the vector between the objects
	glm::vec3 midline = one.getAxis(3) - two.getAxis(3);

	// See if it is large enough
	return glm::dot(midline, midline) < (one.radius + two.radius) * (one.radius + two.radius);
}

// This preprocessor definition is only used as a convenience
// in the boxAndBox intersection method
#define TEST_OVERLAP(axis) overlapOnAxis(one, two, (axis), toCentre)
bool CGProj::CGCollisionEarlyExit::OBBAndOBB(const CGCollisionOBB & one, const CGCollisionOBB & two)
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

bool CGProj::CGCollisionEarlyExit::OBBAndHalfSpace(const CGCollisionOBB & box, const CGCollisionPlane & plane)
{
	// Work out the projected radius of the box onto the plane direction
	GPED::real projectedRadius = transformToAxis(box, plane.direction);

	// Work out how far the box is from the origin
	GPED::real boxDistance = glm::dot(plane.direction, box.getAxis(3)) - plane.offset;

	// Check for the intersection
	return real_abs(boxDistance) <= projectedRadius;
}
