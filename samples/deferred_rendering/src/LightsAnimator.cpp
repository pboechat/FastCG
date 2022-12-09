#include "LightsAnimator.h"

#include <FastCG/Transform.h>
#include <FastCG/Random.h>

using namespace FastCG;

FASTCG_IMPLEMENT_COMPONENT(LightsAnimator, Behaviour);

void LightsAnimator::SetLights(const std::vector<PointLight *> &rLights)
{
	mLights = rLights;
	mDirections.clear();
	Random::SeedWithTime();
	for (auto *pLight : mLights)
	{
		mDirections.emplace_back(glm::normalize(glm::vec3(Random::NextFloat() - 0.5f, 0, Random::NextFloat() - 0.5f)));
	}
}

void LightsAnimator::ChangeColors()
{
	for (auto *pLight : mLights)
	{
		glm::vec4 color = Random::NextColor();
		pLight->SetDiffuseColor(color);
		pLight->SetSpecularColor(color);
	}
}

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
	if (time - mLastDirectionChangeTime > 5.0f)
	{
		for (size_t i = 0; i < mLights.size(); i++)
		{
			mDirections[i] = mDirections[i] * -1.0f;
		}
		mLastDirectionChangeTime = time;
	}
	else
	{
		for (size_t i = 0; i < mLights.size(); i++)
		{
			auto *pLight = mLights[i];
			glm::vec3 move = mDirections[i] * deltaTime;
			glm::vec3 position = pLight->GetGameObject()->GetTransform()->GetPosition();
			pLight->GetGameObject()->GetTransform()->SetPosition(position + move);
		}
	}
}
