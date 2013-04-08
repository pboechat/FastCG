#ifndef COLORMATCHINGFUNCTIONS_H
#define COLORMATCHINGFUNCTIONS_H

#include "LightSpectrum.h"
#include <CIEXYZColor.h>

class ColorMatchingFunctions
{
public:
	static CIEXYZColor Apply(const LightSpectrum& rLightSpectrum);

private:
	static const unsigned int STARTING_WAVELENGTH;
	static const unsigned int ENDING_WAVELENGTH;

	static const float xBarFunction[];
	static const float yBarFunction[];
	static const float zBarFunction[];

	ColorMatchingFunctions() {}
	~ColorMatchingFunctions() {}

};

#endif