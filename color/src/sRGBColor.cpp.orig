#include <sRGBColor.h>
#include <CIEXYZColor.h>
#include <ColorExceptions.h>

#include <cmath>

double sRGBColor::InverseGammaCorrection(float x)
{
	double ft;
	double t = (double) (x > 0) ? x : -x;

	if (t > 0.04045)
	{
		ft = pow((t + 0.055) / 1.055, 2.4);
	}
	else
	{
		ft = t / 12.92;
	}

	return (x > 0) ? ft : -ft;
}

double sRGBColor::GammaCorrection(double x)
{
	double ft;
	double t = (x > 0) ? x : -x;

	if (t > 0.0031308)
	{
		ft = 1.055 * pow(t, 1.0 / 2.4) - 0.055;
	}
	else
	{
		ft = 12.92 * t;
	}

	return (x > 0) ? ft : -ft;
}

CIEXYZColor sRGBColor::ToCIEXYZ() const
{
	double Rc = InverseGammaCorrection(mR);
	double Gc = InverseGammaCorrection(mG);
	double Bc = InverseGammaCorrection(mB);

	float X = (float) (Rc * 0.4124564 + Gc * 0.3575761 + Bc * 0.1804375);
	float Y = (float) (Rc * 0.2126729 + Gc * 0.7151522 + Bc * 0.0721750);
	float Z = (float) (Rc * 0.0193339 + Gc * 0.1191920 + Bc * 0.9503041);

	if (X < 0 || Y < 0 || Z < 0)
	{
		//THROW_EXCEPTION(InvalidColorConversionException, "Invalid conversion from sRGB to CIEXYZ");
		InvalidColorConversionException exception("Invalid conversion from sRGB to CIEXYZ");
		exception.SetFunction(__FUNCTION__);
		exception.SetFile(__FILE__);
		exception.SetLine(__LINE__);
		throw exception;
	}

	return CIEXYZColor(X, Y, Z);
}
