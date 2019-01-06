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

	inline bool tryAxis
	(
		const CGCollisionOBB& OBB,
		const glm::vec3& v0,
		const glm::vec3& v1,
		const glm::vec3& v2,
		glm::vec3 axis,
		unsigned index,

		// These values may be updated
		GPED::real& smallestPenetration,
		unsigned& smallestCase
	)
	{
		// Make sure we have a normalized axis and don't check almost parallel axes
		if (glm::dot(axis, axis) < 0.0001) return true;
		axis = glm::normalize(axis);
		
		GPED::real axisRadius = transformToAxis(OBB, axis);
		GPED::real p0 = glm::dot(v0, axis);
		GPED::real p1 = glm::dot(v1, axis);
		GPED::real p2 = glm::dot(v2, axis);

		GPED::real Length = GPED::rMax(-GPED::rMax(p0, p1, p2), GPED::rMin(p0, p1, p2));
		GPED::real penetration = axisRadius - Length;

		if (penetration < 0) return false;
		if (penetration < smallestPenetration)
		{
			smallestPenetration = penetration;
			smallestCase = index;
		}
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

	// RTCD 136 ~ 142
	inline glm::vec3 ClosestPtPointTriangle(const glm::vec3& sCenter, const CGCollisionTriangle& triangle)
	{
		// Check if P in vertex region outside A
		glm::vec3 ab = triangle.m_points[1] - triangle.m_points[0];
		glm::vec3 ac = triangle.m_points[2] - triangle.m_points[0];
		glm::vec3 ap = sCenter - triangle.m_points[0];
		GPED::real d1 = glm::dot(ab, ap);
		GPED::real d2 = glm::dot(ac, ap);
		if (d1 <= GPED::real(0.0) && d2 <= GPED::real(0.0)) return triangle.m_points[0]; // barycentric coordinates (1,0,0)

		// Check if P in vertex region outside B
		glm::vec3 bp = sCenter - triangle.m_points[1];
		GPED::real d3 = glm::dot(ab, bp);
		GPED::real d4 = glm::dot(ac, bp);
		if (d3 >= GPED::real(0.0) && d4 <= d3) return triangle.m_points[1]; // barycentric coordinates (0,1,0)

		// Check if P in edge region of AB, if so return projection P onto AB
		GPED::real vc = d1 * d4 - d3 * d2;
		if (vc <= GPED::real(0.0) && d1 >= GPED::real(0.0) && d3 <= GPED::real(0.0))
		{
			GPED::real v = d1 / (d1 - d3);
			return triangle.m_points[0] + v * ab;  // barycentric coordinates (1-v, v, 0)
		}

		// Check if P in vertex region outside C
		glm::vec3 cp = sCenter - triangle.m_points[2];
		GPED::real d5 = glm::dot(ab, cp);
		GPED::real d6 = glm::dot(ac, cp);
		if (d6 >= GPED::real(0.0) && d5 <= d6) return triangle.m_points[2]; // barycentric coordinate (0, 0, 1)

		// Check if P in edge region of AC, if so return projection of P onto AC
		GPED::real vb = d5 * d2 - d1 * d6;
		if (vb <= GPED::real(0.0) && d2 >= GPED::real(0.0) & d6 <= GPED::real(0.0))
		{
			GPED::real w = d2 / (d2 - d6);
			return triangle.m_points[0] + w * ac; // barycentric coordinates (1-w, 0, w)
		}

		// Check if P in edge region of BC, if so return projection of P onto BC
		GPED::real va = d3 * d6 - d5 * d4;
		if (va <= GPED::real(0.0) && (d4 - d3) >= GPED::real(0.0) && (d5 - d6) >= GPED::real(0.0))
		{
			GPED::real w = (d4 - d3) / (d4 - d3) + (d5 - d6);
			return triangle.m_points[1] + w * (triangle.m_points[2] - triangle.m_points[1]); // barycentric coordinates (0, 1 - w, w)
		}

		// P inside face region. COmpute Q through its barycentric coordinates (u,v,w)
		GPED::real denom = GPED::real(1.0) / (va + vb + vc);
		GPED::real v = vb * denom;
		GPED::real w = vc * denom;
		return triangle.m_points[0] + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
	}

	// Get Normal Version
	inline glm::vec3 ClosestPtPointTriangle(const glm::vec3& sCenter, const CGCollisionTriangle& triangle, glm::vec3& outNormal)
	{
		// Check if P in vertex region outside A
		glm::vec3 ab = triangle.m_points[1] - triangle.m_points[0];
		glm::vec3 ac = triangle.m_points[2] - triangle.m_points[0];

		outNormal.x = ab.y * ac.z - ab.z * ac.y;
		outNormal.y = ab.z * ac.x - ab.x * ac.z;
		outNormal.z = ab.x * ac.y - ab.y * ac.x;
		outNormal = glm::normalize(outNormal);

		glm::vec3 ap = sCenter - triangle.m_points[0];
		GPED::real d1 = glm::dot(ab, ap);
		GPED::real d2 = glm::dot(ac, ap);
		if (d1 <= GPED::real(0.0) && d2 <= GPED::real(0.0)) return triangle.m_points[0]; // barycentric coordinates (1,0,0)

		// Check if P in vertex region outside B
		glm::vec3 bp = sCenter - triangle.m_points[1];
		GPED::real d3 = glm::dot(ab, bp);
		GPED::real d4 = glm::dot(ac, bp);
		if (d3 >= GPED::real(0.0) && d4 <= d3) return triangle.m_points[1]; // barycentric coordinates (0,1,0)

		// Check if P in edge region of AB, if so return projection P onto AB
		GPED::real vc = d1 * d4 - d3 * d2;
		if (vc <= GPED::real(0.0) && d1 >= GPED::real(0.0) && d3 <= GPED::real(0.0))
		{
			GPED::real v = d1 / (d1 - d3);
			return triangle.m_points[0] + v * ab;  // barycentric coordinates (1-v, v, 0)
		}

		// Check if P in vertex region outside C
		glm::vec3 cp = sCenter - triangle.m_points[2];
		GPED::real d5 = glm::dot(ab, cp);
		GPED::real d6 = glm::dot(ac, cp);
		if (d6 >= GPED::real(0.0) && d5 <= d6) return triangle.m_points[2]; // barycentric coordinate (0, 0, 1)

		// Check if P in edge region of AC, if so return projection of P onto AC
		GPED::real vb = d5 * d2 - d1 * d6;
		if (vb <= GPED::real(0.0) && d2 >= GPED::real(0.0) & d6 <= GPED::real(0.0))
		{
			GPED::real w = d2 / (d2 - d6);
			return triangle.m_points[0] + w * ac; // barycentric coordinates (1-w, 0, w)
		}

		// Check if P in edge region of BC, if so return projection of P onto BC
		GPED::real va = d3 * d6 - d5 * d4;
		if (va <= GPED::real(0.0) && (d4 - d3) >= GPED::real(0.0) && (d5 - d6) >= GPED::real(0.0))
		{
			GPED::real w = (d4 - d3) / (d4 - d3) + (d5 - d6);
			return triangle.m_points[1] + w * (triangle.m_points[2] - triangle.m_points[1]); // barycentric coordinates (0, 1 - w, w)
		}

		// P inside face region. COmpute Q through its barycentric coordinates (u,v,w)
		GPED::real denom = GPED::real(1.0) / (va + vb + vc);
		GPED::real v = vb * denom;
		GPED::real w = vc * denom;
		return triangle.m_points[0] + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
	}
}

#endif