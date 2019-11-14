#ifndef __CG_MATH_CONFIG_H__
#define __CG_MATH_CONFIG_H__

namespace CGProj
{
#define SINGLE_PRECISION
#define DOUBLE_PRECISIOn

#if defined(SINGLE_PRECISION)
	typedef float real;
#elif defined(DOUBLE_PRECISION)
	typedef double real;
#else
	#error You Should Specify the precision preprocessor
#endif
}

#endif
