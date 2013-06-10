#ifndef LIGHTSANIMATOR_H_
#define LIGHTSANIMATOR_H_

#include <Behaviour.h>
#include <PointLight.h>

#include <glm/glm.hpp>

#include <vector>

COMPONENT(LightsAnimator, Behaviour)
public:
	void OnInstantiate()
	{
		mLastDirectionChangeTime = 0;

		mLastMouseButtonPressTime = 0;
	}

	void SetLights(const std::vector<PointLight*>& rLights);

	virtual void OnUpdate(float time, float deltaTime);

private:
	std::vector<PointLight*> mPointLights;
	std::vector<glm::vec3> mDirections;
	float mLastDirectionChangeTime;

	float mLastMouseButtonPressTime;

};

#endif