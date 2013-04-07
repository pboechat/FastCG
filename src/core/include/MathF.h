#ifndef MATHF_H
#define MATHF_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <stdlib.h>

class MathF
{
public:
	static const float DEGREES_TO_RADIANS;
	static const float PI;
	static const float TWO_PI;

	inline static bool IsPowerOfTwo(unsigned int i)
	{
		return i && !(i & (i - 1));
	}

	inline static unsigned int UpperPowerOfTwo(unsigned int i)
	{
		i--;
		i |= i >> 1;
		i |= i >> 2;
		i |= i >> 4;
		i |= i >> 8;
		i |= i >> 16;
		i++;

		return i;
	}

	inline static float Sqrt(float a)
	{
		return sqrt(a);
	}

	inline static float Ceil(float a)
	{
		return ceil(a);
	}

	inline static float Log2(float a)
	{
		return log(a) / log(2.0f);
	}

	inline static float Pow(float a, int b)
	{
		return pow(a, b);
	}

	inline static float Max(float a, float b)
	{
		if (a >= b) return a;
		else return b;
	}

	inline static float Min(float a, float b)
	{
		if (a <= b) return a;
		else return b;
	}

	inline static float Cos(float angleInRadians)
	{
		return cos(angleInRadians);
	}

	inline static float Sin(float angleInRadians)
	{
		return sin(angleInRadians);
	}

	inline static float Tan(float angleInRadians)
	{
		return tan(angleInRadians);
	}

	inline static float Abs(float value)
	{
		return abs(value);
	}

private:
	MathF();
	~MathF();
	
};

#endif
