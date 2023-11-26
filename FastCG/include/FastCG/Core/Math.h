#ifndef FASTCG_MATH_H
#define FASTCG_MATH_H

#define _USE_MATH_DEFINES

#include <cstdlib>
#include <math.h>

namespace FastCG
{
	template <typename T>
	class Math final
	{
	public:
		static const float DEGREES_TO_RADIANS;
		static const float RADIANS_TO_DEGREES;
		static const float PI;
		static const float TWO_PI;

		inline static bool IsPowerOfTwo(unsigned int i);
		inline static unsigned int UpperPowerOfTwo(unsigned int i);
		inline static T Round(T a);
		inline static T Sqrt(T a);
		inline static T Ceil(T a);
		inline static T Floor(T a);
		inline static T Log2(T a);
		inline static T Pow(T a, int b);
		inline static T Max(T a, T b);
		inline static T Min(T a, T b);
		inline static T Cos(T angleInRadians);
		inline static T Sin(T angleInRadians);
		inline static T Tan(T angleInRadians);
		inline static T Abs(T value);
		inline static T Clamp(T value, T low, T high);
		inline static T FMod(T a, T b);
		inline static T Lerp(T a, T b, T c);
		inline static T Sign(T a);

	private:
		Math() = delete;
		~Math() = delete;
	};

	template <typename T>
	const float Math<T>::DEGREES_TO_RADIANS = 0.0174533f;
	template <typename T>
	const float Math<T>::RADIANS_TO_DEGREES = 57.2958f;
	template <typename T>
	const float Math<T>::PI = (float)M_PI;
	template <typename T>
	const float Math<T>::TWO_PI = (float)(2.0 * M_PI);

	template <typename T>
	inline bool Math<T>::IsPowerOfTwo(unsigned int i)
	{
		return i && !(i & (i - 1));
	}

	template <typename T>
	inline unsigned int Math<T>::UpperPowerOfTwo(unsigned int i)
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

	template <typename T>
	inline T Math<T>::Round(T a)
	{
		double number = (double)a;
		return (T)((number < 0.0) ? ceil(number - 0.5) : floor(number + 0.5));
	}

	template <typename T>
	inline T Math<T>::Sqrt(T a)
	{
		return (T)sqrt(a);
	}

	template <typename T>
	inline T Math<T>::Ceil(T a)
	{
		return (T)ceil(a);
	}

	template <typename T>
	inline T Math<T>::Floor(T a)
	{
		return (T)floor(a);
	}

	template <typename T>
	inline T Math<T>::Log2(T a)
	{
		return (T)log(a) / (T)log(2.0f);
	}

	template <typename T>
	inline T Math<T>::Pow(T a, int b)
	{
		return (T)pow(a, b);
	}

	template <typename T>
	inline T Math<T>::Max(T a, T b)
	{
		if (a >= b)
		{
			return a;
		}

		else
		{
			return b;
		}
	}

	template <typename T>
	inline T Math<T>::Min(T a, T b)
	{
		if (a <= b)
		{
			return a;
		}

		else
		{
			return b;
		}
	}

	template <typename T>
	inline T Math<T>::Cos(T angleInRadians)
	{
		return (T)cos(angleInRadians);
	}

	template <typename T>
	inline T Math<T>::Sin(T angleInRadians)
	{
		return (T)sin(angleInRadians);
	}

	template <typename T>
	inline T Math<T>::Tan(T angleInRadians)
	{
		return (T)tan(angleInRadians);
	}

	template <typename T>
	inline T Math<T>::Abs(T value)
	{
		return (T)abs(value);
	}

	template <typename T>
	inline T Math<T>::Clamp(T value, T low, T high)
	{
		return value < low ? low : (value > high ? high : value);
	}

	template <typename T>
	inline T Math<T>::FMod(T a, T b)
	{
		return (T)fmod(a, b);
	}

	template <typename T>
	inline T Math<T>::Lerp(T a, T b, T c)
	{
		return a + (b - a) * c;
	}

	template <typename T>
	inline T Math<T>::Sign(T a)
	{
		if (a < 0)
			return -1;
		else
			return 1;
	}

	typedef Math<float> MathF;
	typedef Math<int> MathI;
	typedef Math<unsigned int> MathUI;

}

#endif
