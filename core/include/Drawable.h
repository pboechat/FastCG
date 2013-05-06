#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <Pointer.h>

class Drawable
{
public:
	virtual void Draw() = 0;

};

typedef Pointer<Drawable> DrawablePtr;

#endif