#ifndef __GPED_RANDOM_H__
#define __GPED_RANDOM_H__

#include <GPED/GPED_Precision.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GPED
{
	/**
	 * Keep track of one random stream: i.e. a seed and its output.
	 * This is used to get random numbers. Rather than a function, this
	 * allows there to be several streams of repeatable random numbers
	 * at the same time. Uses the RandRotB algorithm
	 */

	class Random
	{
	public:
		/**
		 * Creats a new random number stream with a seed based on
		 * timing data
		*/
		Random();

		/**
		 * Creates a new random stream with the given seed.
		 */
		Random(unsigned seed);

		/**
		 * Sets the seed value for the random stream
		 */
		void seed(unsigned seed);

		/**
		* left bitwise rotation
		*/
		unsigned rotl(unsigned n, unsigned r);

		/**
		* right bitwise rotation
		*/
		unsigned rotr(unsigned n, unsigned r);

		/**
		 * Returns the next random bitstring from the stream. This is
		 * the fastest method
		 */
		unsigned randomBits();

		/**
		 * Returns a random floating point number between 0 and 1.
		 */
		real randomReal();

		/**
		 * Returns a random floating point number between min and max
		 */
		real randomReal(real min, real max);

		/**
		* Returns a random floating point number between 0 and scale.
		*/
		real randomReal(real scale);

		/**
		 * Returns a random integer less than the given value.
		 */
		unsigned randomInt(unsigned max);

		/**
		 * Returns a random binomially distributed number between -scale
		 * and +scale
		 */
		real randomBinomial(real scale);

		/**
		 * Returns a random vector where each component is binomially
		 * distributed in the range (-scale to scale) [mean = 0.0f]
		 */
		glm::vec3 randomVector(real scale);

		/**
		 * Returns a random vector where each component is binomially
		 * distributed in the range (-scale to scale) [mean = 0.0f],
		 * where scale is the corresponding component to the given vector
		 */
		glm::vec3 randomVector(const glm::vec3& scale);

		/**
		 * Returns a random vector in the cube defined by the given
		 * minimum and maximum vectors. The probability is uniformly 
		 * distributed in this region
		 */
		glm::vec3 randomVector(const glm::vec3& min, const glm::vec3& max);

		/**
		 * Returns a random vector where each component is binomially
		 * distributed in the range (-scale to scale) [mean = 0.0f],
		 * except the coordinate which is zero.
		 */
		glm::vec3 randomXZVector(real scale);

		/**
		 * Returns a random orientation (i.e., normalized) quaternion.
		 */
		glm::quat randomQuaternion();
	
	private:
		// Internal mechanics
		int p1, p2;
		unsigned buffer[17];
	};
}

#endif