#include "LightsAnimator.h"

#include <FastCG/Transform.h>
#include <FastCG/Random.h>
#include <FastCG/MouseButton.h>
#include <FastCG/InputSystem.h>

using namespace FastCG;

IMPLEMENT_COMPONENT(LightsAnimator, Behaviour);

void LightsAnimator::SetLights(const std::vector<PointLight *> &rLights)
{
	mPointLights = rLights;
	mDirections.clear();
	Random::SeedWithTime();
	for (auto *pPointLight : mPointLights)
	{
		mDirections.emplace_back(glm::normalize(glm::vec3(Random::NextFloat() - 0.5f, 0, Random::NextFloat() - 0.5f)));
	}
}

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
	if (time - mLastDirectionChangeTime > 5.0f)
	{
		for (size_t i = 0; i < mPointLights.size(); i++)
		{
			mDirections[i] = mDirections[i] * -1.0f;
		}
		mLastDirectionChangeTime = time;
	}
	else
	{
		for (size_t i = 0; i < mPointLights.size(); i++)
		{
			auto *pPointLight = mPointLights[i];
			glm::vec3 move = mDirections[i] * deltaTime;
			glm::vec3 position = pPointLight->GetGameObject()->GetTransform()->GetPosition();
			pPointLight->GetGameObject()->GetTransform()->SetPosition(position + move);
		}
	}

	if (InputSystem::GetMouseButton(MouseButton::MIDDLE_BUTTON) == MouseButtonState::PRESSED && time - mLastMouseButtonPressTime > 0.333f)
	{
		for (auto *pPointLight : mPointLights)
		{
			glm::vec4 color = Random::NextColor();
			pPointLight->SetDiffuseColor(color);
			pPointLight->SetSpecularColor(color);
		}
		mLastMouseButtonPressTime = time;
	}
}
