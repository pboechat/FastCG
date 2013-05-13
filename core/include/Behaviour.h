#ifndef BEHAVIOUR_H_
#define BEHAVIOUR_H_

#include <Component.h>

ABSTRACT_COMPONENT(Behaviour, Component)
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

#endif