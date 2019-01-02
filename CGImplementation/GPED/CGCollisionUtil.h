#pragma once
#ifndef __CG_COLLISION_UTIL_H__
#define __CG_COLLISION_UTIL_H__

#include <GPED/GPED_Precision.h>
#include <GPED/CGCollisionOBB.h>

namespace CGProj
{
	inline GPED::real transformToAxis(
		const CGCollisionOBB& box,
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
	inline bool overlapOnAxis(
		const CGCollisionOBB& one,
		const CGCollisionOBB& two,
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

	/**
	* This function checks if the two boxes overlap
	* along the given axes, returning the amount of overlap
	* The final parameter toCentre
	* is used to pass in the vector between the boxes entre
	* points, to avoid having to recalculate it each time.
	*/
	inline GPED::real penetrationOnAxis(
		const CGCollisionOBB& one,
		const CGCollisionOBB& two,
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

	inline bool tryAxis(
		const CGCollisionOBB& one,
		const CGCollisionOBB& two,
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

	// Refer to RTCD ClosestPtSegmentSegment
	inline glm::vec3 contactPoint(
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
}

#endif