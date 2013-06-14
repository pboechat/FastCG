#ifndef LIGHTSANIMATOR_H_
#define LIGHTSANIMATOR_H_

#include <Behaviour.h>
#include <GameObject.h>

#include <vector>

COMPONENT(LightsAnimator, Behaviour)
public:
	inline void SetSceneLights(const std::vector<GameObject*>& rSceneLights)
	{
		mSceneLights = rSceneLights;
	}

protected:
	virtual void OnUpdate(float time, float deltaTime);

private:
	std::vector<GameObject*> mSceneLights;

};

#endif