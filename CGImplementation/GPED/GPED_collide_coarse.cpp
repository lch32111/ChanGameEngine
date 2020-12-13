#include <CGPrecompiled.h>
#include "GPED_collide_coarse.h"
using namespace GPED;

GPED::BoundingSphere::BoundingSphere(const glm::vec3 & centre, real radius)
	:centre(centre), radius(radius)
{ }

GPED::BoundingSphere::BoundingSphere(const BoundingSphere & one, const BoundingSphere & two)
{
	glm::vec3 centreOffset = two.centre - one.centre;
	real distance = glm::dot(centreOffset, centreOffset);
	real radiusDiff = two.radius - one.radius;

	// Check if the larger sphere encloses the small one
	if (radiusDiff * radiusDiff >= distance)
	{
		if (one.radius > two.radius)
		{
			centre = one.centre;
			radius = one.radius;
		}
		else
		{
			centre = two.centre;
			radius = two.radius;
		}
	}

	// Otherwise we need to work with partially
	// overlapping spheres
	else
	{
		distance = real_sqrt(distance);
		radius = (distance + one.radius + two.radius) * ((real)0.5);

		// The new centre is based on one's centre, moved towards
		// two's centre by an amount proportional to the spheres'
		// radius.
		centre = one.centre;
		if (distance > 0)
		{
			centre += centreOffset * (radius - one.radius) / distance;
		}
	}

}

bool GPED::BoundingSphere::overlaps(const BoundingSphere * other) const
{
	glm::vec3 centreOffset = centre - other->centre;
	real distanceSquared = glm::dot(centreOffset, centreOffset);
	return distanceSquared < (radius + other->radius) * (radius + other->radius);
}

real GPED::BoundingSphere::getGrowth(const BoundingSphere & other) const
{
	BoundingSphere newSphere(*this, other);

	// We return a value proportional to the change in surface
	// area of the sphere.
	return newSphere.radius * newSphere.radius - radius * radius;
}
