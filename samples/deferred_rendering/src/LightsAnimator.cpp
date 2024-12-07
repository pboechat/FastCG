#include "LightsAnimator.h"

#include <FastCG/Core/Random.h>
#include <FastCG/World/Transform.h>
#include <FastCG/World/WorldSystem.h>

using namespace FastCG;

FASTCG_IMPLEMENT_COMPONENT(LightsAnimator, Behaviour);

void LightsAnimator::ChangeColors()
{
    for (auto *pPointLight : mPointLights)
    {
        auto color = Random::NextColor();
        pPointLight->SetDiffuseColor(color);
        pPointLight->SetSpecularColor(color);
    }
}

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
    if (!mInitialized)
    {
        WorldSystem::GetInstance()->FindComponents<PointLight>(mPointLights);

        mDirections.resize(mPointLights.size());
        for (size_t i = 0; i < mPointLights.size(); ++i)
        {
            mDirections[i] = glm::normalize(glm::vec3(Random::NextFloat() - 0.5f, 0, Random::NextFloat() - 0.5f));
        }
        ChangeColors();
        mInitialized = true;
    }

    if (!mEnabled)
    {
        return;
    }

    if (time - mLastDirectionChangeTime > 5.0f)
    {
        for (size_t i = 0; i < mDirections.size(); i++)
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
            auto move = mDirections[i] * deltaTime;
            auto position = pPointLight->GetGameObject()->GetTransform()->GetWorldPosition();
            pPointLight->GetGameObject()->GetTransform()->SetPosition(position + move);
        }
    }
}
