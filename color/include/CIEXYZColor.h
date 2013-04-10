#ifndef CIEXYZCOLOR_H
#define CIEXYZCOLOR_H

#include "Color.h"
#include "ColorModel.h"
#include "Illuminant.h"

class sRGBColor;

class CIEXYZColor: public Color
{
public:
	// White references
	static const CIEXYZColor A;
	static const CIEXYZColor C;
	static const CIEXYZColor D50;
	static const CIEXYZColor D55;
	static const CIEXYZColor D65;
	static const CIEXYZColor D75;
	static const CIEXYZColor F2;
	static const CIEXYZColor F7;
	static const CIEXYZColor F11;
	static const CIEXYZColor test;

	CIEXYZColor()
	{
	}

	CIEXYZColor(float X, float Y, float Z) :
			mX(X), mY(Y), mZ(Z)
	{
	}

	virtual ~CIEXYZColor()
	{
	}

	virtual ColorModel GetModel() const
	{
		return CM_CIEXYZ;
	}

	inline float X() const
	{
		return mX;
	}

	inline float Y() const
	{
		return mY;
	}

	inline float Z() const
	{
		return mZ;
	}

	void operator =(const CIEXYZColor& rOther)
	{
		mX = rOther.mX;
		mY = rOther.mY;
		mZ = rOther.mZ;
	}

	float operator [](int index)
	{
		if (index == 0)
		{
			return mX;
		}
		else if (index == 1)
		{
			return mY;
		}
		else if (index == 2)
		{
			return mZ;
		}
		else
		{
			return 0;
		}
	}

	sRGBColor To_sRGB(bool ignoreError = false) const;
	sRGBColor To_sRGB(Illuminant referenceLight, bool ignoreError = false) const;

private:
	float mX;
	float mY;
	float mZ;

};

#endif
