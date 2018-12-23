#pragma once
#ifndef __CG_DEF_SECOND_UTIL_H__
#define __CG_DEF_SECOND_UTIL_H__

/** 2018-12-12 Chanhaeng Lee
 * This header file specifies the various things of DeferredSecond.fs to
 * make use of the macro, variables, etc, on the shader
 */

// The number of G-buffer texture
#define NR_GBUFFER_TEXTURES 5

 // Limit of the array size for each kind of light
#define NR_DIR_LIGHTS 3
#define NR_DIR_SHADOWS 2

#define NR_POINT_LIGHTS 15
#define NR_POINT_SHADOWS 4

#define NR_SPOT_LIGHTS 10
#define NR_SPOT_SHADOWS 4

#define SHADOW_INDEX_NONE -1

#endif
