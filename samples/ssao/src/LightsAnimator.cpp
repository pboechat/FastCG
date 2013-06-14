#include "LightsAnimator.h"

#include <Transform.h>

COMPONENT_IMPLEMENTATION(LightsAnimator, Behaviour);

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
	for (unsigned int i = 0; i < mSceneLights.size(); i++)
	{
		mSceneLights[i]->GetTransform()->RotateAround(deltaTime * 100.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	}
}