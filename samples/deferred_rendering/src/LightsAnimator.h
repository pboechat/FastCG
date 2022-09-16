#ifndef LIGHTS_ANIMATOR_H_
#define LIGHTS_ANIMATOR_H_

#include <FastCG/Behaviour.h>
#include <FastCG/PointLight.h>

#include <glm/glm.hpp>

using namespace FastCG;

class LightsAnimator : public Behaviour
{
	DECLARE_COMPONENT(LightsAnimator, Behaviour);

public:
	void OnInstantiate() override
	{
		mLastDirectionChangeTime = 0;
		mLastMouseButtonPressTime = 0;
	}

	void SetLights(const std::vector<PointLight*>& rLights);
	void OnUpdate(float time, float deltaTime) override;

private:
	std::vector<PointLight*> mPointLights;
	std::vector<glm::vec3> mDirections;
	float mLastDirectionChangeTime;
	float mLastMouseButtonPressTime;

};

#endif