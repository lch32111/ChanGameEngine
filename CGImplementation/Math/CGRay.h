#ifndef __CG_RAY_H__
#define __CG_RAY_H__

#include <Math/CGVector3.h>

namespace CGProj
{
	namespace Math
	{
		class CGRay
		{
		public:
			CGRay() { }
			CGVector3 startPoint;
			CGVector3 direction;
		};
	}
}

#endif
