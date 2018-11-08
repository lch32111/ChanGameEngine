#ifndef __GPED_UTILS_H__
#define __GPED_UTILS_H__

#include <limits>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GPED
{
#if 1
	/**
	* Defines we're in single precision mode, for any code
	* that needs to be conditionally compiled.
	*/
#define SINGLE_PRECISION

	/**
	* Defines a real number precision. Cyclone can be compiled in
	* single or double precision versions. By default single precision is
	* provided.
	*/
	typedef float real;

	/** Defines the highest value for the real number. */
#define REAL_MAX FLT_MAX

	/** Defines the precision of the square root operator. */
#define real_sqrt sqrtf
	/** Defines the precision of the absolute magnitude operator. */
#define real_abs fabsf
	/** Defines the precision of the sine operator. */
#define real_sin sinf

	/** Defines the precision of the cosine operator. */
#define real_cos cosf

	/** Defines the precision of the exponent operator. */
#define real_exp expf
	/** Defines the precision of the power operator. */
#define real_pow powf

	/** Defines the precision of the floating point modulo operator. */
#define real_fmod fmodf

	/** Defines the number e on which 1+e == 1 **/
#define real_epsilon FLT_EPSILON

#define R_PI 3.14159f

#else
#define DOUBLE_PRECISION
	typedef double real;
#define REAL_MAX DBL_MAX
#define real_sqrt sqrt
#define real_abs fabs
#define real_sin sin
#define real_cos cos
#define real_exp exp
#define real_pow pow
#define real_fmod fmod
#define real_epsilon DBL_EPSILON
#define R_PI 3.14159265358979
#endif


	/**
	 * Holds the value for energy under which a body will be put to
	 * sleep. This is a global value for the whole solution. By
	 * default it is 0.1, which is fine for simulation when gravity is
	 * about 20 units per second squared, masses are about one, and
	 * other forces are around that of gravity. It may need tweaking
	 * if your simulation is drastically different to this.
	 */
	extern real sleepEpsilon;

	/**
	 * Sets the current sleep epsilon value: the kinetic energy under
	 * which a body may be put to sleep. Bodies are put to sleep if
	 * they appear to have a stable kinetic energy less than this
	 * value. For simulations that often hvave low values (such as slow
	 * moving, or light objects), this may need reducing.
	 *
	 * The value is global; all bodies will use it.
	 *
	 * @see sleepEpsilon
	 *
	 * @see getSleepEpsilon
	 *
	 * @param value The sleep epsilon value to use from this point
	 * on.
	 */
	void setSleepEsilon(real value);

	/**
	 * Gets the current value of the sleep epsilon parameter.
	 *
	 * @see sleepEpsilon
	 *
	 * @see setSleepEpsilon
	 *
	 * @return The current value of the parameter.
	 */
	real getSleepEpsilon();

	glm::mat3 getSkewSymmetric(const glm::vec3& v);
	void getSkewSymmetric(const glm::vec3& v, glm::mat3& mat);

	glm::mat3 getInertiaTensorCoeffs(real ix, real iy, real iz,
		real ixy = 0, real ixz = 0, real iyz = 0);
	void getInertiaTensorCoeffs(glm::mat3& mat, real ix, real iy, real iz,
		real ixy = 0, real ixz = 0, real iyz = 0);

	glm::mat3 getBlockInertiaTensor(const glm::vec3& halfSizes, real mass);
}


#endif
