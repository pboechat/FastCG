#ifndef COLORMATCHINGFUNCTIONS_H
#define COLORMATCHINGFUNCTIONS_H

#include "LightSpectrum.h"
#include "CIEXYZColor.h"
#include <MathF.h>

class ColorMatchingFunctions
{
public:
	static CIEXYZColor Apply(const LightSpectrum& rIlluminanceSpectrum, const LightSpectrum& rReflectanceSpectrum);

private:
	static const unsigned int STARTING_WAVELENGTH;
	static const unsigned int ENDING_WAVELENGTH;

	// CIE 1931 2-deg CMFs
	static const float xBarFunction[];
	static const float yBarFunction[];
	static const float zBarFunction[];

	struct XYZ
	{
		float X;
		float Y;
		float Z;

		XYZ()
		{
			X = 0;
			Y = 0;
			Z = 0;
		}

		XYZ(float X, float Y, float Z)
		{
			this->X = X;
			this->Y = X;
			this->Z = X;
		}

		XYZ operator / (float scalar)
		{
			XYZ other(X, Y, Z);
			other.X /= scalar;
			other.Y /= scalar;
			other.Z /= scalar;
			return other;
		}

		void Normalize()
		{
			float length = Length();
			X /= length;
			Y /= length;
			Z /= length;
		}

		float Length()
		{
			return MathF::Sqrt((X * X) + (Y * Y) + (Z * Z));
		}
	};

	ColorMatchingFunctions() {}
	~ColorMatchingFunctions() {}

	static ColorMatchingFunctions::XYZ PreCalculateXYZ(const LightSpectrum& rLightSpectrum);

};

#endif