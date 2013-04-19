#ifndef COLOR_H_
#define COLOR_H_

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
