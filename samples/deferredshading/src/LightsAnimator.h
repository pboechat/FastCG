#ifndef LIGHTSANIMATOR_H_
#define LIGHTSANIMATOR_H_

#include <Behaviour.h>
#include <Light.h>

#include <glm/glm.hpp>

#include <vector>

COMPONENT(LightsAnimator, Behaviour)
public:
	void SetLights(const std::vector<Light*>& rLights);

	virtual void OnUpdate(float time, float deltaTime);

private:
	std::vector<Light*> mLights;
	std::vector<glm::vec3> mRotationAxis;

};

#endif