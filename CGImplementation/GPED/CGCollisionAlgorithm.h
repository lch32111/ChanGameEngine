#pragma once
#ifndef __CG_COLLISION_ALGORITHM_H__
#define __CG_COLLISION_ALGORITHM_H__

#include <GPED/CGCollisionPrimitive.h>
#include <GPED/CGCollisionNarrow.h>
#include <GPED/CGContactManager.h>

namespace CGProj
{
	/** 2019-01-03 Chanhaeng Lee
	 * This class is used to find the proper algorithm for each primitive type
	 * Specifically, This class is for the narrow phae, not rayCallback.
	 * CGCollisionNarrow already have RayCollisionCallback.
	 * The reason why I didn't insert the callback into here is that
	 * RayCollisionCallback is not that complex. 
	 * It just checks the primitive type, and then executes a proper collision func very quickly.
	 */
	class CGCollisionAlgorithm
	{
	public:
		
		CGCollisionAlgorithm()
		{
			for (unsigned i = 0; i < 3; ++i) m_NarrowAlgoMatrix[0][i] = -1;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_SPHERE][COLLISION_PRIMITIVE_SPHERE] = 0;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_SPHERE][COLLISION_PRIMITIVE_OBB] = 1;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_OBB][COLLISION_PRIMITIVE_SPHERE] = 2;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_OBB][COLLISION_PRIMITIVE_OBB] = 3;

			fPtrSS = CGCollisionNarrow::sphereAndSphere;
			fPtrOO = CGCollisionNarrow::OBBAndOBB;
			fPtrOS = CGCollisionNarrow::OBBAndSphere;
		}

		unsigned findNarrowAlgorithmAndProcess(const CGCollisionPrimitive* a, const CGCollisionPrimitive* b,
			CGContactManager* data)
		{
			int key = m_NarrowAlgoMatrix[a->m_primitiveType][a->m_primitiveType];

			switch (key)
			{
			case 0:
				return fPtrSS(*(CGCollisionSphere*)a, *(CGCollisionSphere*)b, data);
			case 1:
				return fPtrOS(*(CGCollisionOBB*)b, *(CGCollisionSphere*)a, data);
			case 2:
				return fPtrOS(*(CGCollisionOBB*)a, *(CGCollisionSphere*)b, data);
			case 3:
				return fPtrOO(*(CGCollisionOBB*)a, *(CGCollisionOBB*)b, data);
			default:
				// Something wrong happend;
				assert(0);
			}
		}
	private:
		typedef unsigned(*fPtrSphereAndSphere)
			(const CGCollisionSphere&,
			const CGCollisionSphere&,
			CGContactManager*);
		fPtrSphereAndSphere fPtrSS;

		typedef unsigned(*fPtrOBBandOBB)
			(const CGCollisionOBB&,
			const CGCollisionOBB&,
			CGContactManager*);
		fPtrOBBandOBB fPtrOO;

		typedef unsigned(*fPtrOBBandSphere)
			(const CGCollisionOBB&,
				const CGCollisionSphere&,
				CGContactManager*);
		fPtrOBBandSphere fPtrOS;

		int m_NarrowAlgoMatrix[3][3];
		/*   
		 * Collision_PRIMITIVE_SPHERE == 1
		 * Collision_PRIMITIVE_SPHERE == 2
		 *         Empty Sphere OBB 
		 *  Empty   -1        
		 *  Sphere  -1     0      1 
		 *  OBB     -1     2      3
		*/
	};
}

#endif