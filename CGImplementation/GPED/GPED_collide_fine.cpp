#include "GPED_collide_fine.h"
#include <iostream>

static inline GPED::real transformToAxis(
	const GPED::CollisionBox& box,
	const glm::vec3& axis
)
{
	/*
	 This Trnsform means Local to World Space Conversion.
	 Think about this P(W<-L) = [ u0 u1 u2 ], and then.
	 the local X axis is V_x(halfSize.x, 0, 0). To convert this local into world space,
	 [u0 u1 u2] * V_x == glm::dot(box.getAxis(0), V_x), because the V_X compontes except x component are zero.
	 So, only the u0 components will be used to get world space X axis. and then we need to the dot produc withe the given axis.
	 Refer to the RTCD Books on TestOBBPlane. You can see the details how to derive these codes.
	*/
	return
		box.halfSize.x * real_abs(glm::dot(axis, box.getAxis(0))) +
		box.halfSize.y * real_abs(glm::dot(axis, box.getAxis(1))) +
		box.halfSize.z * real_abs(glm::dot(axis, box.getAxis(2)));
}

/**
 * This function checks if the two boxes overlap
 * along the given axis. The final parameter toCentre
 * is used to pass in the vector between the boxes centre
 * points, to avoid having to recalculate it each time.
 */
static inline bool overlapOnAxis(
	const GPED::CollisionBox& one,
	const GPED::CollisionBox& two,
	const glm::vec3& axis,
	const glm::vec3& toCentre
)
{
	// Project the half-size of one onto axis
	GPED::real oneProject = transformToAxis(one, axis);
	GPED::real twoProject = transformToAxis(two, axis);

	// Project this onto the axis
	GPED::real distance = real_abs(glm::dot(toCentre, axis));

	// Check for overlap
	return (distance < oneProject + twoProject); 
}

void GPED::CollisionPrimitive::calculateInternals()
{
	transform = body->getTransform();
}

bool GPED::IntersectionTests::sphereAndHalfSpace(const CollisionSphere & sphere, const CollisionPlane & plane)
{
	// Find the distance from the origin
	real ballDistance = glm::dot(plane.direction, sphere.getAxis(3)) - sphere.radius;

	// Check for the intersection
	return ballDistance <= plane.offset;
}

bool GPED::IntersectionTests::sphereAndSphere(const CollisionSphere & one, const CollisionSphere & two)
{
	// Find the vector between the objects
	glm::vec3 midline = one.getAxis(3) - two.getAxis(3);

	// See if it is large enough
	return glm::dot(midline, midline) < (one.radius + two.radius) * (one.radius + two.radius);
}

// This preprocessor definition is only used as a convenience
// in the boxAndBox intersection method
#define TEST_OVERLAP(axis) overlapOnAxis(one, two, (axis), toCentre)
bool GPED::IntersectionTests::boxAndBox(const CollisionBox & one, const CollisionBox & two)
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

bool GPED::IntersectionTests::boxAndHalfSpace(const CollisionBox & box, const CollisionPlane & plane)
{
	// Work out the projected radius of the box onto the plane direction
	real projectedRadius = transformToAxis(box, plane.direction);

	// Work out how far the box is from the origin
	real boxDistance = glm::dot(plane.direction, box.getAxis(3)) - plane.offset;
	
	// Check for the intersection
	return real_abs(boxDistance) <= projectedRadius;
}


unsigned GPED::CollisionDetector::sphereAndHalfSpace(
	const CollisionSphere & sphere, 
	const CollisionPlane & plane, 
	ContactManager * data)
{
	// Cache the sphere position
	glm::vec3 position = sphere.getAxis(3);

	// Find the distance from the plane
	real ballDistance = glm::dot(plane.direction, position) - plane.offset - sphere.radius ;

	if (ballDistance >= 0) return 0;

	// Create the contact - it has a normal in the plane direction.
	int contactId = data->GetEmptyContactNode();
	Contact* contact = data->GetFirstContact();
	contact->contactNormal = plane.direction;
	contact->penetration = -ballDistance;
	contact->contactPoint = position - plane.direction * (ballDistance + sphere.radius);
	data->setBodyData(contactId, sphere.body, NULL);
	return 1;
}

unsigned GPED::CollisionDetector::sphereAndTruePlane(
	const CollisionSphere & sphere, 
	const CollisionPlane & plane, 
	ContactManager * data)
{
	// Cache the sphere position
	glm::vec3 position = sphere.getAxis(3);

	// Find the distance from the plane
	real centreDistance = glm::dot(plane.direction, position) - plane.offset;

	// Check if we're within radius
	if (centreDistance * centreDistance > sphere.radius * sphere.radius)
	{
		return 0;
	}

	// Check which side of the plane we're on
	glm::vec3 normal = plane.direction;
	real penetration = -centreDistance;
	if (centreDistance < 0)
	{
		normal *= -1;
		penetration = -penetration;
	}
	penetration += sphere.radius;

	// Create the contact - it has a normal in the plane direction
	int contactId = data->GetEmptyContactNode();
	Contact* contact = data->GetFirstContact();
	contact->contactNormal = normal;
	contact->penetration = penetration;
	contact->contactPoint = position - plane.direction * centreDistance;
	data->setBodyData(contactId, sphere.body, NULL);

	return 1;
}

unsigned GPED::CollisionDetector::sphereAndSphere(
	const CollisionSphere & one, 
	const CollisionSphere & two, 
	ContactManager * data)
{
	// Cache the sphere positions
	glm::vec3 positionOne = one.getAxis(3);
	glm::vec3 positionTwo = two.getAxis(3);

	// Find the vector between the objects
	glm::vec3 midline = positionOne - positionTwo;
	real size = glm::length(midline);

	// See if it is large enough.
	if (size <= 0.0f || size >= one.radius + two.radius)
	{
		return 0;
	}

	// We manually create the normal, because we have the
	// size to hand.
	glm::vec3 normal = midline * ((real)(1.0) / size);

	int contactId = data->GetEmptyContactNode();
	Contact* contact = data->GetFirstContact();
	contact->contactNormal = normal;
	contact->contactPoint = positionOne + midline * (real) 0.5;
	contact->penetration = (one.radius + two.radius - size);
	data->setBodyData(contactId, one.body, two.body);
	
	return 1;
}

unsigned GPED::CollisionDetector::boxAndHalfSpace(
	const CollisionBox & box, 
	const CollisionPlane & plane, 
	ContactManager * data)
{
	// Check for intersection
	if (!IntersectionTests::boxAndHalfSpace(box, plane)) return 0;

	// We have an intersection, so find the intersection points, we can make
	// do with only checking vertices. If the box is resting on a plane
	// or on an edge, it will be reported as four or two contact points.

	// Go through each combination of + and - for each half-size
	static real mults[8][3] = { { 1,1,1 },{ -1,1,1 },{ 1,-1,1 },{ -1,-1,1 },
								{ 1,1,-1 },{ -1,1,-1 },{ 1,-1,-1 },{ -1,-1,-1 } };

	unsigned contactUsed = 0;
	for (unsigned i = 0; i < 8; ++i)
	{
		// Calculate the position of each vertex
		glm::vec3 vertexPos(mults[i][0], mults[i][1], mults[i][2]);
		vertexPos = vertexPos * box.halfSize;
		vertexPos = glm::vec3(box.transform * glm::vec4(vertexPos, 1.0));

		// Calculate the distance from the plane
		real vertexDistance = glm::dot(vertexPos, plane.direction);

		// Compare this to the plane's distance
		if (vertexDistance <= plane.offset)
		{
			// Create the contact data.

			// The contact point is halfway between the vertex and the
			// plane - we multiply the direction by half the separation
			// distance and add the vertex location.
			int contactId = data->GetEmptyContactNode();
			Contact* contact = data->GetFirstContact();
			contact->contactPoint = vertexPos + 
				plane.direction * (vertexDistance - plane.offset);
			contact->contactNormal = plane.direction;
			contact->penetration = plane.offset - vertexDistance;

			// Write the appropriate data
			data->setBodyData(contactId, box.body, NULL);
			
			contactUsed++;
		}
	}
	
	return contactUsed;
}

/**
 * This function checks if the two boxes overlap
 * along the given axes, returning the amount of overlap
 * The final parameter toCentre
 * is used to pass in the vector between the boxes entre
 * points, to avoid having to recalculate it each time.
 */
static inline GPED::real penetrationOnAxis(
	const GPED::CollisionBox& one,
	const GPED::CollisionBox& two,
	const glm::vec3& axis,
	const glm::vec3& toCentre
)
{
	// Project the half-size of one onto axis
	// It means transform world coordinate of each 
	GPED::real oneProject = transformToAxis(one, axis);
	GPED::real twoProject = transformToAxis(two, axis);

	// Project this onto the axis
	GPED::real distance = real_abs(glm::dot(toCentre, axis));

	// Return the overlap (i.e. positive indicates
	// overlap, negative indicates separation).
	return oneProject + twoProject - distance;
}

static inline bool tryAxis(
	const GPED::CollisionBox& one,
	const GPED::CollisionBox& two,
	glm::vec3 axis,
	const glm::vec3& toCentre,
	unsigned index,

	// These values may be updated
	GPED::real& smallestPenetration,
	unsigned& smallestCase
)
{
	// Make sure we have a normalized axis and don't check almost parallel axes
	if (glm::dot(axis, axis) < 0.0001) return true;
	axis = glm::normalize(axis);

	GPED::real penetration = penetrationOnAxis(one, two, axis, toCentre);
	if (penetration < 0) return false;
	if (penetration < smallestPenetration)
	{
		smallestPenetration = penetration;
		smallestCase = index;
	}
	return true;
}

void fillPointFaceBoxBox(
	const GPED::CollisionBox& one,
	const GPED::CollisionBox& two,
	const glm::vec3& toCentre,
	GPED::ContactManager* data,
	unsigned best,
	GPED::real pen
)
{
	// This method is called when we know that a vertex from
	// box two is in contact with box one
	int contactId = data->GetEmptyContactNode();
	GPED::Contact* contact = data->GetFirstContact();

	// We know which axis the collision is on (i.e best),
	// but we need to work out which of the two faces on
	// this axis.
	glm::vec3 normal = one.getAxis(best);
	if (glm::dot(normal, toCentre) > 0)
	{
		normal = normal * GPED::real(-1.0);
	}

	// Work out which vertex of box two we're colliding with
	// Using toCentre doesn't work!
	glm::vec3 vertex = two.halfSize;
	if (glm::dot(two.getAxis(0), normal) < 0) vertex.x = -vertex.x;
	if (glm::dot(two.getAxis(1), normal) < 0) vertex.y = -vertex.y;
	if (glm::dot(two.getAxis(2), normal) < 0) vertex.z = -vertex.z;
	// Create the contact data
	contact->contactNormal = normal;
	contact->penetration = pen;
	contact->contactPoint = glm::vec3(two.getTransform() * glm::vec4(vertex, 1.0));
	data->setBodyData(contactId, one.body, two.body);
}

// Refer to RTCD ClosestPtSegmentSegment
static inline glm::vec3 contactPoint(
	const glm::vec3& pOne,
	const glm::vec3& dOne,
	GPED::real oneSize,
	const glm::vec3& pTwo,
	const glm::vec3& dTwo,
	GPED::real twoSize,

	// If this is true, and the contact point is outside
	// the edge (in the case of an edge-face contact) then
	// we use one's midpoint, otherwise we use two's
	bool useOne
)
{
	glm::vec3 toSt, cOne, cTwo;
	GPED::real dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
	GPED::real denom, mua, mub;

	smOne = glm::dot(dOne, dOne);
	smTwo = glm::dot(dTwo, dTwo);
	dpOneTwo = glm::dot(dTwo, dOne);

	toSt = pOne - pTwo;
	dpStaOne = glm::dot(dOne, toSt);
	dpStaTwo = glm::dot(dTwo, toSt);

	denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	// Zero denominator indicates parallel lines
	if (real_abs(denom) < 0.0001f)
	{
		return useOne ? pOne : pTwo;
	}

	mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
	mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

	// If either of the edges has the nearse point out
	// of bounds, then the edges aren't crossed, we have
	// an edge-face contact. Our point is on the edge. which
	// we know from the useOne parameter.
	if (mua > oneSize || mua < -oneSize || mub > twoSize || mub < -twoSize)
		return useOne ? pOne : pTwo;
	else
	{
		cOne = pOne + dOne * mua;
		cTwo = pTwo + dTwo * mub;
		return cOne * GPED::real(0.5) + cTwo * GPED::real(0.5);
	}
}

// This preprocessor definition is only used as a convenience
// in the boxAndBox contact generation method
#define CHECK_OVERLAP(axis, index) \
	if(!tryAxis(one, two, (axis), toCentre, (index), pen, best)) return 0;

unsigned GPED::CollisionDetector::boxAndBox(
	const CollisionBox & one, 
	const CollisionBox & two, 
	ContactManager * data)
{
	// if(!IntersectionTest::boxAndBox(one, two)) return 0;

	// Find the vector between the two centres
	glm::vec3 toCentre = two.getAxis(3) - one.getAxis(3);

	// We start  assuming there is no contact
	real pen = REAL_MAX;
	unsigned best = 0xffffff;

	// Now we check each axes, returning if it gives us
	// a separating axis, and keeping track of the axis with
	// the smallest penetration otherwise.
	
	CHECK_OVERLAP(one.getAxis(0), 0);
	CHECK_OVERLAP(one.getAxis(1), 1);
	CHECK_OVERLAP(one.getAxis(2), 2);

	CHECK_OVERLAP(two.getAxis(0), 3);
	CHECK_OVERLAP(two.getAxis(1), 4);
	CHECK_OVERLAP(two.getAxis(2), 5);

	// Store the best axis-major. in case we run into almost
	// parallel edge collisions later
	unsigned bestSingleAxis = best;

	CHECK_OVERLAP(glm::cross(one.getAxis(0), two.getAxis(0)), 6);
	CHECK_OVERLAP(glm::cross(one.getAxis(0), two.getAxis(1)), 7);
	CHECK_OVERLAP(glm::cross(one.getAxis(0), two.getAxis(2)), 8);
	CHECK_OVERLAP(glm::cross(one.getAxis(1), two.getAxis(0)), 9);
	CHECK_OVERLAP(glm::cross(one.getAxis(1), two.getAxis(1)), 10);
	CHECK_OVERLAP(glm::cross(one.getAxis(1), two.getAxis(2)), 11);
	CHECK_OVERLAP(glm::cross(one.getAxis(2), two.getAxis(0)), 12);
	CHECK_OVERLAP(glm::cross(one.getAxis(2), two.getAxis(1)), 13);
	CHECK_OVERLAP(glm::cross(one.getAxis(2), two.getAxis(2)), 14);

	// Make sure we've got a result
	assert(best != 0xffffff);

	// We now know there's a collision, and we know which 
	// of the axes gave the smallest penetration. We now
	// can deal with it in different ways depending on
	// the case
	if (best < 3)
	{
		// We've got a vertex of box two on a face of box one.
		fillPointFaceBoxBox(one, two, toCentre, data, best, pen);
		return 1;
	}
	else if (best < 6)
	{
		// We've got a vertex of box one on a face of box two.
		// We use the same algorithm as above, but swap around
		// one and two (and therefore also the vector between their
		// centres).
		fillPointFaceBoxBox(two, one, toCentre * real(-1.0), data, best - 3, pen);
		return 1;
	}
	else
	{
		// We've got an edge-edge contact. Find out which axes
		best -= 6;
		unsigned oneAxisIndex = best / 3;
		unsigned twoAxisIndex = best % 3;
		glm::vec3 oneAxis = one.getAxis(oneAxisIndex);
		glm::vec3 twoAxis = two.getAxis(twoAxisIndex);
		glm::vec3 axis = glm::normalize(glm::cross(oneAxis, twoAxis));
		// The axis should point from box one to box two
		if (glm::dot(axis, toCentre) > 0) axis = axis * real(-1.0);

		// We have the axes, but not the edges: each axis has 4 edges parallel
		// to it, we need to find which of the 4 for each object. We do
		// that by finding the point in the centre of the edge. We know
		// its component in the direction of the box's collision axis is zero
		// (its a mid-point) and we determine which of the extremes in each
		// of the oher axes in closest
		glm::vec3 ptOnOneEdge = one.halfSize;
		glm::vec3 ptOnTwoEdge = two.halfSize;
		for(unsigned i = 0; i < 3; ++i)
		{
			if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
			else if (glm::dot(one.getAxis(i), axis) > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

			if (i == oneAxisIndex) ptOnTwoEdge[i] = 0;
			else if (glm::dot(two.getAxis(i), axis) > 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
		}

		// Move them into world coordinates (they are already oriented
		// Correctly, since they have been dreived from the axes).
		ptOnOneEdge = glm::vec3(one.transform * glm::vec4(ptOnOneEdge, 1.0));
		ptOnTwoEdge = glm::vec3(two.transform * glm::vec4(ptOnTwoEdge, 1.0));

		// So we have a point and a direction for the colliding edges.
		// We need to find out point of closest approach of the two
		// line-segments
		glm::vec3 vertex = contactPoint(
			ptOnOneEdge, oneAxis, one.halfSize[oneAxisIndex],
			ptOnTwoEdge, twoAxis, two.halfSize[twoAxisIndex],
			bestSingleAxis > 2
		);

		// We can fill the contact
		int contactId = data->GetEmptyContactNode();
		Contact* contact = data->GetFirstContact();
		contact->penetration = pen;
		contact->contactNormal = axis;
		contact->contactPoint = vertex;
		data->setBodyData(contactId, one.body, two.body);
		return 1;
		
	}
	
	return 0;
}
#undef CHECK_OVERLAP

unsigned GPED::CollisionDetector::boxAndPoint(
	const CollisionBox & box, 
	const glm::vec3 & point, 
	ContactManager * data)
{
	// Transform the point into box coordinates
	glm::vec3 relPt = glm::transpose(glm::mat3(box.transform)) * 
					  (point - glm::vec3(box.transform[3]));

	glm::vec3 normal;

	// Check each axis, looking for the axis on which the
	// penetration is least deep
	real min_depth = box.halfSize.x - real_abs(relPt.x);
	if (min_depth < 0) return 0;
	normal = box.getAxis(0) * ((relPt.x < 0) ? real(-1) : real(1));

	real depth = box.halfSize.y - real_abs(relPt.y);
	if (depth < 0) return 0;
	else if (depth < min_depth)
	{
		min_depth = depth;
		normal = box.getAxis(1) * ((relPt.y < 0) ? real(-1) : real(1));
	}

	depth = box.halfSize.z - real_abs(relPt.z);
	if (depth < 0) return 0;
	else if (depth < min_depth)
	{
		min_depth = depth;
		normal = box.getAxis(2) * ((relPt.z < 0) ? real(-1) : real(1));
	}

	// Compile the contact
	int contactId = data->GetEmptyContactNode();
	Contact* contact = data->GetFirstContact();
	contact->contactNormal = normal;
	contact->contactPoint = point;
	contact->penetration = min_depth;

	// Noe that we don't know what rigid body the point
	// belongs to, so we just use NULL. Where this is called
	// this value can be left or filled in.
	data->setBodyData(contactId, box.body, NULL);

	return 1;
}

#define DIST_EPSILON 0.0001
unsigned GPED::CollisionDetector::boxAndSphere(
	const CollisionBox & box, 
	const CollisionSphere & sphere, 
	ContactManager * data)
{
	// Transform the centre of the sphere into box coordinates
	glm::vec3 sphereCentre = sphere.getAxis(3);
	glm::vec3 relCentre = glm::transpose(glm::mat3(box.transform)) * 
						  (sphereCentre - glm::vec3(box.transform[3]));
	
	// Early out check to see if we can exclude the contact
	if (real_abs(relCentre.x) > sphere.radius + box.halfSize.x ||
		real_abs(relCentre.y) > sphere.radius + box.halfSize.y ||
		real_abs(relCentre.z) > sphere.radius + box.halfSize.z)
	{
		return 0;
	}

	glm::vec3 closestPt(0, 0, 0);
	real dist;

	// Clamp each coordinate to the box
	dist = relCentre.x;
	if (dist > box.halfSize.x) dist = box.halfSize.x;
	if (dist < -box.halfSize.x) dist = -box.halfSize.x;
	closestPt.x = dist;

	dist = relCentre.y;
	if (dist > box.halfSize.y) dist = box.halfSize.y;
	if (dist < -box.halfSize.y) dist = -box.halfSize.y;
	closestPt.y = dist;

	dist = relCentre.z;
	if (dist > box.halfSize.z) dist = box.halfSize.z;
	if (dist < -box.halfSize.z) dist = -box.halfSize.z;
	closestPt.z = dist;

	// Check we're in contact
	dist = glm::dot((closestPt - relCentre), (closestPt - relCentre));
	if (dist > sphere.radius * sphere.radius) return 0;



	/*
	18.11.03 Chanhaeng Lee
	In the case that the sphere is in the box,
	the contact normal will be nan(Not A number) number, garbage value.
	This case destroy the collision system. So, I added the special case.
	The solution is giving the closest point of box hull to the closestPt Point.
	It means the closest point will be one of the vertices of the box.
	*/
	if (dist <= DIST_EPSILON)
	{
		real distX, distY, distZ;
		distX = box.halfSize.x - real_abs(closestPt.x);
		distY = box.halfSize.y - real_abs(closestPt.y);
		distZ = box.halfSize.z - real_abs(closestPt.z);
		if (distX < distY && distX < distZ)
		{
			closestPt.x = closestPt.x > 0 ? box.halfSize.x : -box.halfSize.x;
			// Epsilon for the case that the sphere center is the vertex position of the box
			closestPt.y += 0.006; 
			closestPt.z += 0.006;
		}
		else if (distY < distX && distY < distZ)
		{
			closestPt.y = closestPt.y > 0 ? box.halfSize.y : -box.halfSize.y;
			// Epsilon for the case that the sphere center is the vertex position of the box
			closestPt.x += 0.006;
			closestPt.z += 0.006;
		}
		else
		{
			closestPt.z = closestPt.z > 0 ? box.halfSize.z : -box.halfSize.z;
			// Epsilon for the case that the sphere center is the vertex position of the box
			closestPt.x += 0.006;
			closestPt.y += 0.006;
		}
	}
	
	// Compile the contact
	int contactId = data->GetEmptyContactNode();
	Contact* contact = data->GetFirstContact();
	
	glm::vec3 closestPtWorld = glm::vec3(box.transform * glm::vec4(closestPt, 1.0));
	glm::vec3 sphereToClosestPt = closestPtWorld - sphereCentre;
	
	contact->contactNormal = sphereToClosestPt;
	if (dist <= DIST_EPSILON)
	{
		// 18.11.03 : Because sphere -> box point normal should be reversed.
		// and the dist between sphere and closest pt was changed.
		dist = glm::dot(sphereToClosestPt, sphereToClosestPt);
		contact->contactNormal *= -1;
	}
	dist = real_sqrt(dist);
	contact->contactNormal *= (real(1.0) / dist);
	contact->contactPoint = closestPtWorld;
	contact->penetration = real_abs(sphere.radius - dist);
	data->setBodyData(contactId, box.body, sphere.body);

	return 1;
}

// Intersection between ray and OBB
unsigned GPED::CollisionDetector::rayAndBox(
	GPED::c3RayOutput & output,
	const GPED::c3RayInput & input,
	const CollisionBox & box)
{
	GPED::real tMin = -REAL_MAX;
	GPED::real tMax = REAL_MAX;

	glm::vec3 position = box.getAxis(3);
	glm::vec3 delta = position - input.startPoint;

	c3AABB aabb;
	aabb.min = glm::vec3(-1);
	aabb.max = aabb.min * -1.f;
	for (int i = 0; i < 3; ++i)
	{
		glm::vec3 axis = box.getAxis(i);
		GPED::real e = glm::dot(axis, delta);
		GPED::real f = GPED::real(1.0) / glm::dot(input.direction, axis);

		if (real_abs(f) < REAL_EPSILON)
		{
			if (e < aabb.min[i] || e > aabb.max[i])
				return false;
		}
		else
		{
			GPED::real t1 = (aabb.min[i] - e) * f;
			GPED::real t2 = (aabb.max[i] - e) * f;

			if (t1 > t2) GPED::Swap(t1, t2);
			tMin = GPED::rMax(tMin, t1);
			tMax = GPED::rMin(tMax, t2);
			if (tMin > tMax) return 0;
		}
	}

	output.t = tMin;
	output.startPoint = input.startPoint;
	output.hitPoint = input.startPoint + input.direction * tMin;
	return 1;
}

// RTCD 178p ~ 179p.
unsigned GPED::CollisionDetector::rayAndSphere(
	GPED::c3RayOutput & output,
	const GPED::c3RayInput & input,
	const CollisionSphere & sphere)
{
	glm::vec3 m = sphere.getAxis(3) - input.startPoint;
	GPED::real b = glm::dot(m, input.direction);
	GPED::real c = glm::dot(m, m) - sphere.radius * sphere.radius;
	// Exit if r's origin outside s (c > 0) and r pointing away from  s (b > 0)
	if (c > GPED::real(0.0) && b > GPED::real(0.0)) return 0;
	GPED::real discr = b * b - c;
	// A negative discriminant corresponds to ray missing sphere
	if (discr < GPED::real(0.0)) return 0;
	// Ray now found to intersect sphere, compute smallest t value of intersection
	GPED::real t = -b - real_sqrt(discr);
	// If t is negative, ray started inside sphere so clamp t to zero
	if (t < GPED::real(0.0)) t = GPED::real(0.0);
	output.hitPoint = input.startPoint + t * input.direction;
	output.startPoint = input.startPoint;
	output.t = t;
	return 1;
}


