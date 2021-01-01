#ifndef __CG_PRECOMPILED_H__
#define __CG_PRECOMPILED_H__

// Referred to the https://github.com/PolygonTek/BlueshiftEngine/blob/master/Source/Runtime/Precompiled.h

// detect x86 32 bit platform
#if defined(__i386__) || defined(_M_IX86)
	#if !defined(__X86__)
		#define __X86__
	#endif
	#if !defined(__X86_32__)
		#define __X86_32__
	#endif
#endif

// detect x86 64 bit platform
#if defined(__x86_64__) || defined(__ia64__) || defined(_M_X64)
	#if !defined(__X86__)
		#define __X86__
	#endif
	#if !defined(__X86_64__)
		#define __X86_64__
	#endif
#endif

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(__CYGWIN__)
	#if !defined(__WIN32__)
		#define __WIN32__
	#endif
#endif

#if defined(_DEBUG) && !defined(CG_DEBUG)
	#define CG_DEBUG
#else
	#define CG_RELEASE
#endif

#if defined(CG_DEBUG)
	#if defined(__WIN32__) && defined(_MSC_VER)
		#define _CRT_SECURE_NO_WARNINGS
		#include <iostream>
		#include <assert.h>
		#include <intrin.h>
		#define CG_DEBUG_ASSERT(x) if(!(x)) { std::cout << "Debug Assert | " << __FILE__ << " ( " << __LINE__ << " )", assert((0)); }
		#define CG_DEBUG_BREAK(x) if(!(x)) {std::cout << "Debug Break | " << __FILE__ << " ( " << __LINE__ << " ) ", __debugbreak();}
	#endif
#elif defined(CG_RELEASE)
#define CG_DEBUG_ASSERT(x)
#define CG_DEBUG_BREAK(x)
#endif

#if defined(CG_DEBUG)
	#if defined(__WIN32__) && defined(_MSC_VER)
		#ifndef _CRTDBG_MAP_ALLOC
			#define _CRTDBG_MAP_ALLOC
			#include<stdlib.h>
			#include<crtdbg.h>
			#define CG_DBG_NEW	new (_NORMAL_BLOCK , __FILE__ , __LINE__ )
		#endif
	#endif
#endif

#include <stdint.h>

#include <cmath>
#include <cstdint>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef double float64;

#endif