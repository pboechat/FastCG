#ifndef COLOR_H
#define COLOR_H

#include "ColorModel.h"

class Color
{
public:
	Color()
	{
	}
	;
	virtual ~Color()
	{
	}
	;

	virtual ColorModel GetModel() const = 0;

};

#endif
