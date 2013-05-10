#ifndef BEHAVIOUR_H_
#define BEHAVIOUR_H_

#include <Component.h>
#include <Pointer.h>

class Behaviour : public Component
{
	DECLARE_TYPE;

public:
	inline void Update(float time, float deltaTime)
	{
		if (IsEnabled())
		{
			OnUpdate(time, deltaTime);
		}
	}

protected:
	virtual void OnUpdate(float time, float deltaTime) = 0;

};

typedef Pointer<Behaviour> BehaviourPtr;

#endif