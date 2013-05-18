#include "LightsAnimator.h"

#include <Application.h>
#include <Transform.h>
#include <Random.h>
#include <MathT.h>
#include <FontRegistry.h>

COMPONENT_IMPLEMENTATION(LightsAnimator, Behaviour);

void LightsAnimator::SetLights(const std::vector<PointLight*>& rLights)
{
	mPointLights = rLights;

	mDirections.clear();
	Random::SeedWithTime();
	for (unsigned int i = 0; i < mPointLights.size(); i++)
	{
		PointLight* pPointLight = mPointLights[0];
		mDirections.push_back(glm::normalize(glm::vec3(Random::NextFloat() - 0.5f, 0, Random::NextFloat() - 0.5f)));
	}
}

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
	static char pText[128];
	sprintf(pText, "Animated Lights: %d", mPointLights.size());

#ifdef USE_PROGRAMMABLE_PIPELINE
	static unsigned int text3Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 4 + 9;
	Application::GetInstance()->DrawText(pText, FontRegistry::STANDARD_FONT_SIZE, 10, text3Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
#else
	static unsigned int text1Height = 15;
	Application::GetInstance()->DrawText(pText, 12, 10, text1Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
#endif

	if (time - mLastDirectionChangeTime > 5.0f)
	{
		for (unsigned int i = 0; i < mPointLights.size(); i++)
		{
			mDirections[i] = mDirections[i] * -1.0f;
		}
		mLastDirectionChangeTime = time;
	}
	else
	{
		for (unsigned int i = 0; i < mPointLights.size(); i++)
		{
			PointLight* pPointLight = mPointLights[i];

			glm::vec3 move = mDirections[i] * deltaTime;
			glm::vec3 newPosition = pPointLight->GetGameObject()->GetTransform()->GetWorldPosition() + move;
			pPointLight->GetGameObject()->GetTransform()->SetWorldPosition(newPosition);
		}
	}
}
