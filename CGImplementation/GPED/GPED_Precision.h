#ifndef __GPED_UTILS_H__
#define __GPED_UTILS_H__

#include <limits>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Forward Declaration 
namespace CGProj { class CGCollisionPrimitive; }

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
#define REAL_EPSILON 0.0001
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

	real rMin(real a, real b);
	real rMax(real a, real b);
	glm::vec3 rMin(const glm::vec3& a, const glm::vec3& b);
	glm::vec3 rMax(const glm::vec3& a, const glm::vec3& b);
	template<class T>
	void Swap(T& a, T& b) { T t = a; a = b; b = t; }
	
	/* 18-11-13 Chanhaneg Lee
		I referred to http://www.randygaul.net/2013/08/06/dynamic-aabb-tree/ .
		However, this article is almost based on the Box2D Code.
		So, I also referred to the Box2D Code.
		I made the code work with our project code
	*/
#define aabbExtension 0.01
#define aabbMultiplier 2.0
	struct c3AABB // CG Project 3-dimenstional AABB
	{
		glm::vec3 min;
		glm::vec3 max;

		GPED::real GetPerimeter() const
		{
			GPED::real wx = max.x - min.x;
			GPED::real wy = max.y - min.y;
			GPED::real wz = max.z - min.z;

			return GPED::real(4) * (wx + wy + wz);
		}

		void Combine(const c3AABB& aabb)
		{
			min = GPED::rMin(min, aabb.min);
			max = GPED::rMax(max, aabb.max);
		}

		void Combine(const c3AABB& aabb1, const c3AABB& aabb2)
		{
			min = GPED::rMin(aabb1.min, aabb2.min);
			max = GPED::rMax(aabb1.max, aabb2.max);
		}

		// Does this aabb contain the provided AABB;
		bool Contains(const c3AABB& aabb) const
		{
			bool result = true;
			result = result && min.x <= aabb.min.x;
			result = result && min.y <= aabb.min.y;
			result = result && min.z <= aabb.min.z;
			result = result && aabb.max.x <= max.x;
			result = result && aabb.max.y <= max.y;
			result = result && aabb.max.z <= max.z;
			return result;
		}
	};
	
	struct c3RayInput
	{
		c3RayInput()
			: startPoint(glm::vec3(0)), direction(glm::vec3(0))
		{	}

		c3RayInput(const glm::vec3& rayFrom, const glm::vec3& rayTo)
			: startPoint(rayFrom)
		{
			direction = glm::normalize(rayTo - rayFrom);
		}

		glm::vec3 startPoint;
		glm::vec3 direction;
	};
	
	struct c3RayOutput
	{
		glm::vec3 hitPoint; // literally, hitPoint between ray and object
		GPED::real t; // startPoint + t * direction from rayInput
		glm::vec3 startPoint; // From RayInput
	};

	bool aabbOverlap(const GPED::c3AABB & a, const c3AABB & b);
	bool rayaabbOverlap(const GPED::c3AABB& a, const GPED::c3RayInput& ray);
	bool rayaabbIntersection(GPED::c3RayOutput& output, 
		const GPED::c3RayInput& input, const GPED::c3AABB& aaabb);

	/// The method to enable GPED Object to interact with CGBroadPhase 
	/// which uses dynamicAABB tree
	GPED::c3AABB convertFromCollisionPrimitive(const CGProj::CGCollisionPrimitive& primitive);
	GPED::c3AABB makeAABB(const glm::vec3& position, const glm::vec3& halfExtents); // from AABB
	GPED::c3AABB makeAABB(const glm::mat3& orientation, const glm::vec3& position, const glm::vec3& halfExtents); // from OBB
	GPED::c3AABB makeAABB(const glm::vec3& position, const GPED::real radius); // from Sphere
}


#endif
