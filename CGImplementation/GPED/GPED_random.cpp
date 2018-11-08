#include "GPED_random.h"
#include <cstdlib>
#include <ctime>

using namespace GPED;

GPED::Random::Random()
{
	seed(0);
}

GPED::Random::Random(unsigned seed)
{
	Random::seed(seed);
}

void GPED::Random::seed(unsigned s)
{
	if (s == 0) s = (unsigned)clock();

	// Fill the buffer with some basic random numbers
	for (unsigned i = 0; i < 17; ++i)
	{
		// Simple linear congruential generator
		s = s * 2891336453 + 1;
		buffer[i] = s;
	}

	// Initialize pointers into the buffer
	p1 = 0, p2 = 10;
}

unsigned GPED::Random::rotl(unsigned n, unsigned r)
{
	/*
	the end r bits of n will go to most significant place
	the first 32 - r bits of n will go to least significant place
	so it makes sense to rotate the bits left.
	*/
	
	return (n << r) | (n >> (32 - r));
}

unsigned GPED::Random::rotr(unsigned n, unsigned r)
{
	/* 
	the first r bits of n will go to least significant place
	the end 32 - r bits of n will go to most significant place
	so it makes sense to rotate the bits right.
	*/
	return (n >> r) | (n << (32 - r));
}

unsigned GPED::Random::randomBits()
{
	unsigned result;

	// ROtate the buffer and store it back to itself
	result = buffer[p1] = rotl(buffer[p2], 13) + rotl(buffer[p1], 9);

	// Rotate pointers
	if (--p1 < 0) p1 = 16;
	if (--p2 < 0) p2 = 16;

	// Return result
	return result;
}

#ifdef SINGLE_PRECISION
real GPED::Random::randomReal()
{
	// Get the random number
	unsigned bits = randomBits();

	// Set up a reinterpret structure for manipulation
	union
	{
		real value;
		unsigned word;
	} convert;

	// Now assign the bits to the word. This works by fixing the ieee
	// sign and exponent bits (so the size of the result is 1~2)
	// and using the bits to create the fraction part of the float.

	/*
	IEEE 754 standard for SINGLE PRECISION
	0 : sign bit (0 - positive, 1 - negative)
	1 ~ 8 : exponent (8), bias = 127 (0111 1111)
	9 ~ 31 : fraction/mantissa (23)
	

	0x3f800000 has the exponent of 127
	it means 2^0 multiplication for the standard representation.
	And shifting-right the bits by nine times, 
	the shifted bits guarantee the result range from 1 to 2.

	Notice ! the bits are saved in little-endian normally.
	it means that 8 bits are saved in memory reversely.
	*/
	convert.word = (bits >> 9) | 0x3f800000;

	// And return the value
	return convert.value - 1.f;
}
#else
real GPED::Random::randomReal()
{
	// Get the random number
	unsigned bits = randomBits();

	// Set up a reinterpret structure for manipulation
	union
	{
		real value;
		unsigned words[2];
	} convert;

	// Now assign the bits to the words. This works by fixing the ieee
	// sign and exponent bits (so that the size of the result is 1~2)
	// and using the bits to create the fraction part of the float. Note
	// that bits are used more than once in this process.
	
	/*
	IEEE 754 standard for DOUBLE PRECISION
	0 : sign bit
	1 ~ 11 : exponent(11), bias = 1023 (0011 1111 1111)
	12 ~ 63 : fraction/mantissa (53)
	
	Notice ! the bits are saved in little-endian normally.
	it means that 8 bits are saved in memory reversely.
	*/
	
	convert.words[0] = bits << 20; // Fill in the top 16bits
	// actually the bits of words[0] should be all in the mantissa region because of little endian
	
	convert.words[1] = (bits >> 12) | 0x3FF00000; // And the bottom 20
	// words[1][0, bit] = sign bit
	// words[1][1, 11, bit] = exponent bit
	// words[1][12, 31, bit] = mantissa

	

	// And return the value
	return convert.value - 1.0;
}
#endif

real GPED::Random::randomReal(real min, real max)
{
	return randomReal() * (max - min) + min;
}

real GPED::Random::randomReal(real scale)
{
	return randomReal() * scale;
}

unsigned GPED::Random::randomInt(unsigned max)
{
	return randomBits() % max;
}

real GPED::Random::randomBinomial(real scale)
{
	return (randomReal() - randomReal()) * scale;
}

glm::vec3 GPED::Random::randomVector(real scale)
{
	return glm::vec3(
		randomBinomial(scale),
		randomBinomial(scale), 
		randomBinomial(scale));
}

glm::vec3 GPED::Random::randomVector(const glm::vec3 & scale)
{
	return glm::vec3(
		randomBinomial(scale.x), 
		randomBinomial(scale.y), 
		randomBinomial(scale.z)
		);
}

glm::vec3 GPED::Random::randomVector(const glm::vec3 & min, const glm::vec3 & max)
{
	return glm::vec3(
	randomReal(min.x, max.x),
	randomReal(min.y, max.y), 
	randomReal(min.z, max.z)
	);
}

glm::vec3 GPED::Random::randomXZVector(real scale)
{
	return glm::vec3(
	randomBinomial(scale),
	0,
	randomBinomial(scale)
	);
}

glm::quat GPED::Random::randomQuaternion()
{
	glm::quat q(
	randomReal(),
	randomReal(), 
	randomReal(), 
	randomReal()
	);
	
	q = glm::normalize(q);
	return q;
}

