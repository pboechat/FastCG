#ifndef UPDATEABLE_H_
#define UPDATEABLE_H_

#include <Pointer.h>

class Updateable
{
public:
	virtual void Update(float time, float deltaTime) = 0;

};

typedef Pointer<Updateable> UpdateablePtr;

#endif