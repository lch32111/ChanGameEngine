#pragma once
#ifndef __CG_FRUSTUM_H__
#define __CG_FRUSTUM_H__

namespace CG
{
	struct CGOrthoFrustum
	{
		float left;
		float right;
		float bottom;
		float top;
		float nearP;
		float farP;
	};

	struct CGPerFrustum
	{
		float fov;
		float aspect;
		float nearP;
		float farP;
	};
}

#endif