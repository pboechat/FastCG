#include "LightsAnimator.h"

#include <Transform.h>
#include <Random.h>
#include <MathT.h>

const float LightsAnimator::ONE_HUNDREDTH_OF_TWO_PI = MathF::TWO_PI * 0.001f;

COMPONENT_IMPLEMENTATION(LightsAnimator, Behaviour);

void LightsAnimator::SetLights(const std::vector<PointLight*>& rLights)
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
		pLight->GetGameObject()->GetTransform()->RotateAround(ONE_HUNDREDTH_OF_TWO_PI * deltaTime, mRotationAxis[i]);
	}
}
