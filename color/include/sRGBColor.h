#ifndef SRGBCOLOR_H
#define SRGBCOLOR_H

#include "Color.h"
#include "ColorModel.h"

#include <sstream>

class CIEXYZColor;

class sRGBColor: public Color
{
public:
	static double InverseGammaCorrection(float x);
	static double GammaCorrection(double x);

	sRGBColor()
	{
	}

	sRGBColor(float r, float g, float b) :
			mR(r), mG(g), mB(b)
	{
	}

	virtual ~sRGBColor()
	{
	}

	virtual ColorModel GetModel() const
	{
		return CM_sRGB;
	}

	inline float R() const
	{
		return mR;
	}

	inline float G() const
	{
		return mG;
	}

	inline float B() const
	{
		return mB;
	}

	float operator [](int index) const
	{
		if (index == 0)
		{
			return mR;
		}
		else if (index == 1)
		{
			return mG;
		}
		else if (index == 2)
		{
			return mB;
		}
		else
		{
			return 0;
		}
	}

	CIEXYZColor ToCIEXYZ() const;

	std::string ToString() const
	{
		std::stringstream str;
		str << "[sRGB(R=" << mR << ",G=" << mG << ",B=" << mB << ")]";
		return str.str();
	}

private:
	float mR;
	float mG;
	float mB;

};

#endif
