#ifndef __CG_VECTOR_3_H__
#define __CG_VECTOR_3_H__

#include <Math/CGMathConfig.h>

namespace CGProj
{
	namespace Math
	{
		class CGVector3
		{
		public:
			CGVector3() { }
			CGVector3(real _x, real _y, real _z) : value{ _x, _y,_z } { }

			CGVector3& operator=(const CGVector3& v)
			{
				value[0] = v.value[0];
				value[1] = v.value[1];
				value[2] = v.value[2];
				return *this;
			}

			real value[3];
		};
	}
}

#endif
