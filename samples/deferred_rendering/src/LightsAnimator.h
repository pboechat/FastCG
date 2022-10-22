#ifndef LIGHTS_ANIMATOR_H
#define LIGHTS_ANIMATOR_H

#include <FastCG/Behaviour.h>
#include <FastCG/PointLight.h>

#include <glm/glm.hpp>

#include <vector>

class LightsAnimator : public FastCG::Behaviour
{
	DECLARE_COMPONENT(LightsAnimator, FastCG::Behaviour);

public:
	void SetLights(const std::vector<FastCG::PointLight *> &rLights);

protected:
	void OnUpdate(float time, float deltaTime) override;

private:
	std::vector<FastCG::PointLight *> mPointLights;
	std::vector<glm::vec3> mDirections;
	float mLastDirectionChangeTime{0};
	float mLastMouseButtonPressTime{0};
};

#endif