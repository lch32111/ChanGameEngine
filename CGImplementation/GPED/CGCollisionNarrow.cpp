#include <GPED/CGCollisionNarrow.h>
#include <GPED/CGCollisionEarlyExit.h>
#include <GPED/CGCollisionUtil.h>

unsigned CGProj::CGCollisionNarrow::NarrowCollisionCallback
(const CGCollisionPrimitive * a, const CGCollisionPrimitive * b, CGContactManager * data)
{
	int aKey = 0;
	switch (a->m_primitiveType)
	{
	case CGCollisionPrimitive::primitiveType::primitive_sphere:
		aKey = 0;
		break;
	case CGCollisionPrimitive::primitiveType::primitive_box:
		aKey = 1;
		break;
	default:
		assert(0);
	}

	int bKey = 0;
	switch (b->m_primitiveType)
	{
	case CGCollisionPrimitive::primitiveType::primitive_sphere:
		bKey = 0;
		break;
	case CGCollisionPrimitive::primitiveType::primitive_box:
		bKey = 1;
		break;
	}

	if (aKey & bKey) return OBBAndOBB(*(CGCollisionOBB*)a, *(CGCollisionOBB*)b, data);
	if (!aKey & !bKey) return sphereAndSphere(*(CGCollisionSphere*)a, *(CGCollisionSphere*)b, data);
	if (aKey) return OBBAndSphere(*(CGCollisionOBB*)a, *(CGCollisionSphere*)b, data);
	else return OBBAndSphere(*(CGCollisionOBB*)b, *(CGCollisionSphere*)a, data);
}

unsigned CGProj::CGCollisionNarrow::RayCollisionCallback(
	GPED::c3RayOutput & output, const GPED::c3RayInput & input, const CGCollisionPrimitive * primitive)
{
	assert(primitive);

	int key = 0;
	switch (primitive->m_primitiveType)
	{
	case CGCollisionPrimitive::primitiveType::primitive_sphere:
		key = 0;
		break;
	case CGCollisionPrimitive::primitiveType::primitive_box:
		key = 1;
		break;
	default:
		assert(0);
	}

	if (key) return rayAndOBB(output, input, *(CGCollisionOBB*)primitive);
	else return rayAndSphere(output, input, *(CGCollisionSphere*)primitive);
}

unsigned CGProj::CGCollisionNarrow::sphereAndHalfSpace(
	const CGCollisionSphere & sphere, const CGCollisionPlane & plane, CGContactManager * data)
{
	// Cache the sphere position
	glm::vec3 position = sphere.getAxis(3);

	// Find the distance from the plane
	GPED::real ballDistance = glm::dot(plane.direction, position) - plane.offset - sphere.radius;

	if (ballDistance >= 0) return 0;

	// Create the contact - it has a normal in the plane direction.
	int contactId = data->GetEmptyContactNode();
	GPED::Contact* contact = data->GetFirstContact();
	contact->contactNormal = plane.direction;
	contact->penetration = -ballDistance;
	contact->contactPoint = position - plane.direction * (ballDistance + sphere.radius);
	data->setBodyData(contactId, sphere.body, NULL);
	return 1;
}

unsigned CGProj::CGCollisionNarrow::sphereAndTruePlane(
	const CGCollisionSphere & sphere, const CGCollisionPlane & plane, CGContactManager * data)
{
	// Cache the sphere position
	glm::vec3 position = sphere.getAxis(3);

	// Find the distance from the plane
	GPED::real centreDistance = glm::dot(plane.direction, position) - plane.offset;

	// Check if we're within radius
	if (centreDistance * centreDistance > sphere.radius * sphere.radius)
	{
		return 0;
	}

	// Check which side of the plane we're on
	glm::vec3 normal = plane.direction;
	GPED::real penetration = -centreDistance;
	if (centreDistance < 0)
	{
		normal *= -1;
		penetration = -penetration;
	}
	penetration += sphere.radius;

	// Create the contact - it has a normal in the plane direction
	int contactId = data->GetEmptyContactNode();
	GPED::Contact* contact = data->GetFirstContact();
	contact->contactNormal = normal;
	contact->penetration = penetration;
	contact->contactPoint = position - plane.direction * centreDistance;
	data->setBodyData(contactId, sphere.body, NULL);

	return 1;
}

unsigned CGProj::CGCollisionNarrow::sphereAndSphere(
	const CGCollisionSphere & one, const CGCollisionSphere & two, CGContactManager * data)
{
	// Cache the sphere positions
	glm::vec3 positionOne = one.getAxis(3);
	glm::vec3 positionTwo = two.getAxis(3);

	// Find the vector between the objects
	glm::vec3 midline = positionOne - positionTwo;
	GPED::real size = glm::length(midline);

	// See if it is large enough.
	if (size <= 0.0f || size >= one.radius + two.radius)
	{
		return 0;
	}

	// We manually create the normal, because we have the
	// size to hand.
	glm::vec3 normal = midline * ((GPED::real)(1.0) / size);

	int contactId = data->GetEmptyContactNode();
	GPED::Contact* contact = data->GetFirstContact();
	contact->contactNormal = normal;
	contact->contactPoint = positionOne + midline * (GPED::real) 0.5;
	contact->penetration = (one.radius + two.radius - size);
	data->setBodyData(contactId, one.body, two.body);

	return 1;
}

unsigned CGProj::CGCollisionNarrow::OBBAndHalfSpace(
	const CGCollisionOBB & box, const CGCollisionPlane & plane, CGContactManager * data)
{
	// Check for intersection
	if (!CGCollisionEarlyExit::OBBAndHalfSpace(box, plane)) return 0;

	// We have an intersection, so find the intersection points, we can make
	// do with only checking vertices. If the box is resting on a plane
	// or on an edge, it will be reported as four or two contact points.

	// Go through each combination of + and - for each half-size
	static GPED::real mults[8][3] = { { 1,1,1 },{ -1,1,1 },{ 1,-1,1 },{ -1,-1,1 },
								{ 1,1,-1 },{ -1,1,-1 },{ 1,-1,-1 },{ -1,-1,-1 } };

	unsigned contactUsed = 0;
	for (unsigned i = 0; i < 8; ++i)
	{
		// Calculate the position of each vertex
		glm::vec3 vertexPos(mults[i][0], mults[i][1], mults[i][2]);
		vertexPos = vertexPos * box.halfSize;
		vertexPos = glm::vec3(box.transform * glm::vec4(vertexPos, 1.0));

		// Calculate the distance from the plane
		GPED::real vertexDistance = glm::dot(vertexPos, plane.direction);

		// Compare this to the plane's distance
		if (vertexDistance <= plane.offset)
		{
			// Create the contact data.

			// The contact point is halfway between the vertex and the
			// plane - we multiply the direction by half the separation
			// distance and add the vertex location.
			int contactId = data->GetEmptyContactNode();
			GPED::Contact* contact = data->GetFirstContact();
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

// This preprocessor definition is only used as a convenience
// in the boxAndBox contact generation method
#define CHECK_OVERLAP(axis, index) \
	if(!tryAxis(one, two, (axis), toCentre, (index), pen, best)) return 0;
unsigned CGProj::CGCollisionNarrow::OBBAndOBB(
	const CGCollisionOBB & one, const CGCollisionOBB & two, CGContactManager * data)
{
	// if(!IntersectionTest::boxAndBox(one, two)) return 0;

	// Find the vector between the two centres
	glm::vec3 toCentre = two.getAxis(3) - one.getAxis(3);

	// We start  assuming there is no contact
	GPED::real pen = REAL_MAX;
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
		fillPointFaceBoxBox(two, one, toCentre * GPED::real(-1.0), data, best - 3, pen);
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
		if (glm::dot(axis, toCentre) > 0) axis = axis * GPED::real(-1.0);

		// We have the axes, but not the edges: each axis has 4 edges parallel
		// to it, we need to find which of the 4 for each object. We do
		// that by finding the point in the centre of the edge. We know
		// its component in the direction of the box's collision axis is zero
		// (its a mid-point) and we determine which of the extremes in each
		// of the oher axes in closest
		glm::vec3 ptOnOneEdge = one.halfSize;
		glm::vec3 ptOnTwoEdge = two.halfSize;
		for (unsigned i = 0; i < 3; ++i)
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
		GPED::Contact* contact = data->GetFirstContact();
		contact->penetration = pen;
		contact->contactNormal = axis;
		contact->contactPoint = vertex;
		data->setBodyData(contactId, one.body, two.body);
		return 1;

	}

	return 0;
}
#undef CHECK_OVERLAP

unsigned CGProj::CGCollisionNarrow::OBBAndPoint(
	const CGCollisionOBB & box, const glm::vec3 & point, CGContactManager * data)
{
	// Transform the point into box coordinates
	glm::vec3 relPt = glm::transpose(glm::mat3(box.transform)) *
		(point - glm::vec3(box.transform[3]));

	glm::vec3 normal;

	// Check each axis, looking for the axis on which the
	// penetration is least deep
	GPED::real min_depth = box.halfSize.x - real_abs(relPt.x);
	if (min_depth < 0) return 0;
	normal = box.getAxis(0) * ((relPt.x < 0) ? GPED::real(-1) : GPED::real(1));

	GPED::real depth = box.halfSize.y - real_abs(relPt.y);
	if (depth < 0) return 0;
	else if (depth < min_depth)
	{
		min_depth = depth;
		normal = box.getAxis(1) * ((relPt.y < 0) ? GPED::real(-1) : GPED::real(1));
	}

	depth = box.halfSize.z - real_abs(relPt.z);
	if (depth < 0) return 0;
	else if (depth < min_depth)
	{
		min_depth = depth;
		normal = box.getAxis(2) * ((relPt.z < 0) ? GPED::real(-1) : GPED::real(1));
	}

	// Compile the contact
	int contactId = data->GetEmptyContactNode();
	GPED::Contact* contact = data->GetFirstContact();
	contact->contactNormal = normal;
	contact->contactPoint = point;
	contact->penetration = min_depth;

	// Noe that we don't know what rigid body the point
	// belongs to, so we just use NULL. Where this is called
	// this value can be left or filled in.
	data->setBodyData(contactId, box.body, NULL);

	return 1;
}

void CGProj::CGCollisionNarrow::fillPointFaceBoxBox(
	const CGCollisionOBB & one, const CGCollisionOBB & two, 
	const glm::vec3 & toCentre, CGContactManager * data, unsigned best, GPED::real pen)
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

unsigned CGProj::CGCollisionNarrow::OBBAndSphere(
	const CGCollisionOBB & box, const CGCollisionSphere & sphere, CGContactManager * data)
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
	GPED::real dist;

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
	if (dist <= real_epsilon)
	{
		GPED::real distX, distY, distZ;
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
	GPED::Contact* contact = data->GetFirstContact();

	glm::vec3 closestPtWorld = glm::vec3(box.transform * glm::vec4(closestPt, 1.0));
	glm::vec3 sphereToClosestPt = closestPtWorld - sphereCentre;

	contact->contactNormal = sphereToClosestPt;
	if (dist <= real_epsilon)
	{
		// 18.11.03 : Because sphere -> box point normal should be reversed.
		// and the dist between sphere and closest pt was changed.
		dist = glm::dot(sphereToClosestPt, sphereToClosestPt);
		contact->contactNormal *= -1;
	}
	dist = real_sqrt(dist);
	contact->contactNormal *= (GPED::real(1.0) / dist);
	contact->contactPoint = closestPtWorld;
	contact->penetration = real_abs(sphere.radius - dist);
	data->setBodyData(contactId, box.body, sphere.body);

	return 1;
}

// Intersection between ray and OBB
unsigned CGProj::CGCollisionNarrow::rayAndOBB(
	GPED::c3RayOutput & output, const GPED::c3RayInput & input, const CGCollisionOBB & box)
{
	GPED::real tMin = -REAL_MAX;
	GPED::real tMax = REAL_MAX;

	glm::vec3 position = box.getAxis(3);
	glm::vec3 delta = position - input.startPoint;

	GPED::c3AABB aabb;
	aabb.min = box.halfSize * GPED::real(-1);
	aabb.max = box.halfSize;
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

	output.startPoint = input.startPoint;
	output.t = tMin;
	output.hitPoint = input.startPoint + input.direction * tMin;
	return 1;
}

// RTCD 178p ~ 179p.
unsigned CGProj::CGCollisionNarrow::rayAndSphere(
	GPED::c3RayOutput & output, const GPED::c3RayInput & input, const CGCollisionSphere & sphere)
{
	glm::vec3 m = input.startPoint - sphere.getAxis(3);
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
	output.startPoint = input.startPoint;
	output.hitPoint = input.startPoint + t * input.direction;
	output.t = t;
	return 1;
}
