#include "LightsAnimator.h"

#include <Transform.h>
#include <Random.h>
#include <MathT.h>

COMPONENT_IMPLEMENTATION(LightsAnimator, Behaviour);

void LightsAnimator::SetLights(const std::vector<Light*>& rLights)
{
	mLights = rLights;

	mRotationAxis.clear();
	Random::SeedWithTime();
	for (unsigned int i = 0; i < mLights.size(); i++)
	{
		glm::vec3 rotationAxis(Random::NextFloat() - 0.5f, Random::NextFloat() - 0.5f, Random::NextFloat() - 0.5f);
		rotationAxis = glm::normalize(rotationAxis);
		mRotationAxis.push_back(rotationAxis);
	}
}

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
	for (unsigned int i = 0; i < mLights.size(); i++)
	{
		Light* pLight = mLights[i];
		pLight->GetGameObject()->GetTransform()->RotateAround(MathF::TWO_PI * deltaTime * 0.01f, mRotationAxis[i]);
	}
}
