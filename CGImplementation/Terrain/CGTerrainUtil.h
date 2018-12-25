#pragma once
#ifndef __CG_TERRAIN_UTIL_H__
#define __CG_TERRAIN_UTIL_H__

namespace CGProj
{
	template <typename T = float>
	inline T lerp(const T& lo, const T& hi, const T& t) { return lo * (1 - t) + hi * t; }
}

#endif