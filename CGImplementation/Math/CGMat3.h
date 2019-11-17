#ifndef __CG_MAT3_H__
#define __CG_MAT3_H__

#include <Math/CGVector3.h>

namespace CGProj
{
	namespace Math
	{
		template<typename Scalar>
		class CGMat3
		{
		public:
			CGMat3() { }
			CGMat3(const CGVector3<Scalar>& col1, 
				const CGVector3<Scalar>& col2, 
				const CGVector3<Scalar>& col3)
				: m_col {col1, col2, col3} { }

			CGVector3<Scalar> m_col[3];
		};
	}
}

#endif