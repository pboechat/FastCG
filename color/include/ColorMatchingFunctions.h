#ifndef COLORMATCHINGFUNCTIONS_H_
#define COLORMATCHINGFUNCTIONS_H_

#include "LightSpectrum.h"
#include "CIEXYZColor.h"
#include <MathF.h>

class ColorMatchingFunctions
{
public:
	static CIEXYZColor Apply(Illuminant referenceLight, const LightSpectrum& rReflectanceSpectrum);

private:
	static const unsigned int MINIMUM_WAVELENGTH;
	static const unsigned int MAXIMUM_WAVELENGTH;

	// CIE 1931 2-deg CMFs
	static const float xBarFunction[];
	static const float yBarFunction[];
	static const float zBarFunction[];

	static const float D65Illuminance[];

	ColorMatchingFunctions()
	{
	}
	~ColorMatchingFunctions()
	{
	}

};

#endif
