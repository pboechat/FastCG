#ifndef CIEELAB_H
#define CIEELAB_H

#include "Color.h"
#include "ColorModel.h"

class CIEELab : public Color
{
public:
	CIEELab ()
	{
	}

	virtual ~CIEELab ()
	{
	}

	virtual ColorModel GetModel() const
	{
		return CIEE_Lab;
	}

private:
	float mL;
	float ma;
	float mb;

}

#endif