#ifndef __CG_COLLISION_CONFIG_H__
#define __CG_COLLISION_CONFIG_H__

#include <Math/CGVector2.h>
#include <Math/CGVector3.h>
#include <Math/CGVector4.h>

#include <Math/CGMat2.h>
#include <Math/CGMat3.h>
#include <Math/CGMat4.h>

// For Common
typedef float CGScalar;
typedef CGProj::Math::CGVector2<CGScalar> CGVec2;
typedef CGProj::Math::CGVector3<CGScalar> CGVec3;		
typedef CGProj::Math::CGVector4<CGScalar> CGVec4;
typedef CGProj::Math::CGMat2<CGScalar> CGMat2;
typedef CGProj::Math::CGMat3<CGScalar> CGMat3;
typedef CGProj::Math::CGMat4<CGScalar> CGMat4;
typedef CGProj::Scalar_traits<CGScalar> CGScalarUtil;

// For specifying float scalar
typedef float CGFScalar;					
typedef CGProj::Math::CGVector2<CGFScalar> CGFVec2;
typedef CGProj::Math::CGVector3<CGFScalar> CGFVec3;
typedef CGProj::Math::CGVector4<CGFScalar> CGFVec4;
typedef CGProj::Math::CGMat2<CGFScalar> CGFMat2;
typedef CGProj::Math::CGMat3<CGFScalar> CGFMat3;
typedef CGProj::Math::CGMat4<CGFScalar> CGFMat4;
typedef CGProj::Scalar_traits<CGFScalar> CGFScalarUtil;

// For specifying double scalar
typedef double CGDScalar;					
typedef CGProj::Math::CGVector2<CGDScalar> CGDVec2;
typedef CGProj::Math::CGVector3<CGDScalar> CGDVec3;
typedef CGProj::Math::CGVector4<CGDScalar> CGDVec4;
typedef CGProj::Math::CGMat2<CGDScalar> CGDMat2;
typedef CGProj::Math::CGMat3<CGDScalar> CGDMat3;
typedef CGProj::Math::CGMat4<CGDScalar> CGDMat4;
typedef CGProj::Scalar_traits<CGDScalar> CGDScalarUtil;

#endif