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
			memset(m_NarrowAlgoMatrix, 0, sizeof(uint8_t) * m_algoMatSize * m_algoMatSize);

			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_SPHERE][COLLISION_PRIMITIVE_SPHERE] = 1;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_SPHERE][COLLISION_PRIMITIVE_OBB] = 2;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_SPHERE][COLLISION_PRIMITIVE_MESH] = 3;
				
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_OBB][COLLISION_PRIMITIVE_SPHERE] = 4;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_OBB][COLLISION_PRIMITIVE_OBB] = 5;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_OBB][COLLISION_PRIMITIVE_MESH] = 6;

			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_MESH][COLLISION_PRIMITIVE_SPHERE] = 7;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_MESH][COLLISION_PRIMITIVE_OBB] = 8;
			m_NarrowAlgoMatrix[COLLISION_PRIMITIVE_MESH][COLLISION_PRIMITIVE_MESH] = 9;

			fPtrSS = CGCollisionNarrow::sphereAndSphere;
			fPtrOO = CGCollisionNarrow::OBBAndOBB;
			fPtrOS = CGCollisionNarrow::OBBAndSphere;
			fPtrMS = CGCollisionNarrow::MeshAndSphere;
			fPtrMO = CGCollisionNarrow::MeshAndOBB;
			fPtrMM = CGCollisionNarrow::MeshandMesh;
		}

		unsigned findNarrowAlgorithmAndProcess(const CGCollisionPrimitive* a, const CGCollisionPrimitive* b,
			CGContactManager* data)
		{
			int key = m_NarrowAlgoMatrix[a->m_primitiveType][b->m_primitiveType];

			switch (key)
			{
			case 1:
				return fPtrSS(*(CGCollisionSphere*)a, *(CGCollisionSphere*)b, data);
			case 2:
				return fPtrOS(*(CGCollisionOBB*)b, *(CGCollisionSphere*)a, data);
			case 3:
				return fPtrMS(*(CGCollisionMesh*)b, *(CGCollisionSphere*)a, data);
			case 4:
				return fPtrOS(*(CGCollisionOBB*)a, *(CGCollisionSphere*)b, data);
			case 5:
				return fPtrOO(*(CGCollisionOBB*)a, *(CGCollisionOBB*)b, data);
			case 6:
				return fPtrMO(*(CGCollisionMesh*)b, *(CGCollisionOBB*)a, data);
			case 7:
				return fPtrMS(*(CGCollisionMesh*)a, *(CGCollisionSphere*)b, data);
			case 8:
				return fPtrMO(*(CGCollisionMesh*)a, *(CGCollisionOBB*)b, data);
			case 9:
				return fPtrMM(*(CGCollisionMesh*)a, *(CGCollisionMesh*)b, data);
			default:
				// Something wrong happend;
				assert(0);
			}

			return -1;
		}
	private:
		// typedef of function pointer
		typedef unsigned(*fPtrSphereAndSphere)
			(const CGCollisionSphere&,
			const CGCollisionSphere&,
			CGContactManager*);
		
		typedef unsigned(*fPtrOBBandOBB)
			(const CGCollisionOBB&,
			const CGCollisionOBB&,
			CGContactManager*);
		
		typedef unsigned(*fPtrOBBandSphere)
			(const CGCollisionOBB&,
				const CGCollisionSphere&,
				CGContactManager*);
		
		typedef unsigned(*fPtrMeshAndSphere)
			(const CGCollisionMesh&,
			const CGCollisionSphere&,
			CGContactManager*);
		
		typedef unsigned(*fPtrMeshAndOBB)
			(const CGCollisionMesh&,
			const CGCollisionOBB&,
			CGContactManager*);
		
		typedef unsigned(*fPtrMeshAndMesh)
			(const CGCollisionMesh&,
			const CGCollisionMesh&,
			CGContactManager*);

		fPtrSphereAndSphere fPtrSS;
		fPtrOBBandOBB fPtrOO;
		fPtrOBBandSphere fPtrOS;
		fPtrMeshAndSphere fPtrMS;
		fPtrMeshAndOBB fPtrMO;
		fPtrMeshAndMesh fPtrMM;

		static const int m_algoMatSize = 4;
		uint8_t m_NarrowAlgoMatrix[m_algoMatSize][m_algoMatSize];
		/*
		 * COLLISION_PRIMITIVE_SPHERE == 1
		 * COLLISION_PRIMITIVE_OBB == 2
		 * COLLISION_PRIMITIVE_MESH == 3
		 *         Empty Sphere OBB  Mesh
		 *  Empty    0     0      0     0
		 *  Sphere   0     1      2     3
		 *  OBB      0     4      5     6
		 *  Mesh     0     7      8     9
		*/
	};
}

#endif