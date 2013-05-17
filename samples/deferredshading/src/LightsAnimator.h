#ifndef LIGHTSANIMATOR_H_
#define LIGHTSANIMATOR_H_

#include <Behaviour.h>
#include <PointLight.h>

#include <glm/glm.hpp>

#include <vector>

COMPONENT(LightsAnimator, Behaviour)
public:
	void SetLights(const std::vector<PointLight*>& rLights);

	virtual void OnUpdate(float time, float deltaTime);

private:
	static const float ONE_HUNDREDTH_OF_TWO_PI;

	std::vector<PointLight*> mLights;
	std::vector<glm::vec3> mRotationAxis;

};

#endif