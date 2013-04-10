#include "include/CIEXYZColor.h"
#include "include/sRGBColor.h"
#include "include/ColorExceptions.h"

#include <MathF.h>

// TODO:
const CIEXYZColor CIEXYZColor::A(1, 1, 1);
const CIEXYZColor CIEXYZColor::C(1, 1, 1);
const CIEXYZColor CIEXYZColor::D50(1, 1, 1);
const CIEXYZColor CIEXYZColor::D55(1, 1, 1);
const CIEXYZColor CIEXYZColor::D65(1, 1, 1);
const CIEXYZColor CIEXYZColor::D75(1, 1, 1);
const CIEXYZColor CIEXYZColor::F2(1, 1, 1);
const CIEXYZColor CIEXYZColor::F7(1, 1, 1);
const CIEXYZColor CIEXYZColor::F11(1, 1, 1);
const CIEXYZColor CIEXYZColor::test(1, 1, 1);

sRGBColor CIEXYZColor::To_sRGB(bool ignoreError) const
{
	return To_sRGB(I_D65, ignoreError);
}

sRGBColor CIEXYZColor::To_sRGB(Illuminant referenceLight, bool ignoreError) const
{
	double r, g, b;

	if (referenceLight == I_D50) 
	{  
		r = (3.3921940 * mX) + (-1.6168667 * mY) + (-0.4906146 * mZ);
		g = (-0.9787684 * mX) + (1.9161415 * mY) + (0.0334540 * mZ);
		b = (0.0719453 * mX) + (-0.2289914 * mY) + (1.4052427 * mZ);
	} 
	else 
	{  
		/* default D65 */
		r = (3.2404542 * mX) + (-1.5371385 * mY) + (-0.4985314 * mZ);
		g = (-0.9692660 * mX) + (1.8760108 * mY) + (0.0415560 * mZ);
		b = (0.0556434 * mX) + (-0.2040259 * mY) + (1.0572252 * mZ);
	}

	// apply gamma correction
	r = sRGBColor::GammaCorrection(r);
	g = sRGBColor::GammaCorrection(g);
	b = sRGBColor::GammaCorrection(b);

	if ((r < 0 || g < 0 || b < 0 || r > 1 || g > 1 || b > 1) && !ignoreError)
	{
		//THROW_EXCEPTION(InvalidColorConversionException, "Invalid conversion from CIEXYZ to sRGB");
		InvalidColorConversionException exception("Invalid conversion from CIEXYZ to sRGB");
		exception.SetFunction(__FUNCTION__);
		exception.SetFile(__FILE__);
		exception.SetLine(__LINE__);
		throw exception;
	}

	return sRGBColor((float)r, (float)g, (float)b);
}
