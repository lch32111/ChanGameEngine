#ifndef __CG_COLLISION_CONFIG_H__
#define __CG_COLLISION_CONFIG_H__

#include <Math/CGVector2.h>
#include <Math/CGVector3.h>
#include <Math/CGVector4.h>

#include <Math/CGMat2.h>
#include <Math/CGMat3.h>
#include <Math/CGMat4.h>

namespace CG
{
	// For Common
	typedef float CGScalar;
	typedef CG::CGVector2<CGScalar> CGVec2;
	typedef CG::CGVector3<CGScalar> CGVec3;
	typedef CG::CGVector4<CGScalar> CGVec4;
	typedef CG::CGMat2<CGScalar> CGCMat2;
	typedef CG::CGMat3<CGScalar> CGCMat3;
	typedef CG::CGMat4<CGScalar> CGCMat4;
	typedef CG::CGScalarOp<CGScalar> CGScalarUtil;

	// For specifying float scalar
	typedef float CGFScalar;
	typedef CG::CGVector2<CGFScalar> CGFVec2;
	typedef CG::CGVector3<CGFScalar> CGFVec3;
	typedef CG::CGVector4<CGFScalar> CGFVec4;
	typedef CG::CGMat2<CGFScalar> CGFMat2;
	typedef CG::CGMat3<CGFScalar> CGFMat3;
	typedef CG::CGMat4<CGFScalar> CGFMat4;
	typedef CG::CGScalarOp<CGFScalar> CGFScalarUtil;

	// For specifying double scalar
	typedef double CGDScalar;
	typedef CG::CGVector2<CGDScalar> CGDVec2;
	typedef CG::CGVector3<CGDScalar> CGDVec3;
	typedef CG::CGVector4<CGDScalar> CGDVec4;
	typedef CG::CGMat2<CGDScalar> CGDMat2;
	typedef CG::CGMat3<CGDScalar> CGDMat3;
	typedef CG::CGMat4<CGDScalar> CGDMat4;
	typedef CG::CGScalarOp<CGDScalar> CGDScalarUtil;

	static inline CGScalar GetGeometryEpsilon()
	{
		return 0.00001f;
	}

	// TODO : do more progresses Collision Contact Info
	struct CGContact
	{
		CGScalar penetration;
		CGVec3 normal;
		CGVec3 position;
	};
}

#endif