#include "LightsAnimator.h"

#include <FastCG/Core/Random.h>
#include <FastCG/World/Transform.h>
#include <FastCG/World/WorldSystem.h>

using namespace FastCG;

FASTCG_IMPLEMENT_COMPONENT(LightsAnimator, Behaviour);

void LightsAnimator::ChangeColors()
{
    std::vector<PointLight *> pointLights;
    WorldSystem::GetInstance()->FindComponents<PointLight>(pointLights);
    ChangeColors(pointLights);
}

void LightsAnimator::ChangeColors(const std::vector<PointLight *> &rPointLights)
{
    for (auto *pPointLight : rPointLights)
    {
        auto color = Random::NextColor();
        pPointLight->SetDiffuseColor(color);
        pPointLight->SetSpecularColor(color);
    }
}

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
    std::vector<PointLight *> pointLights;
    bool triedFindPointLights = false;

    if (!mInitialized)
    {
        WorldSystem::GetInstance()->FindComponents<PointLight>(pointLights);
        triedFindPointLights = true;

        mDirections.clear();
        for (size_t i = 0; i < pointLights.size(); ++i)
        {
            mDirections.emplace_back(
                glm::normalize(glm::vec3(Random::NextFloat() - 0.5f, 0, Random::NextFloat() - 0.5f)));
        }
        ChangeColors(pointLights);
        mInitialized = true;
    }

    if (!mEnabled)
    {
        return;
    }

    if (!triedFindPointLights)
    {
        WorldSystem::GetInstance()->FindComponents<PointLight>(pointLights);
    }

    if (time - mLastDirectionChangeTime > 5.0f)
    {
        for (size_t i = 0; i < pointLights.size(); i++)
        {
            mDirections[i] = mDirections[i] * -1.0f;
        }
        mLastDirectionChangeTime = time;
    }
    else
    {
        for (size_t i = 0; i < pointLights.size(); i++)
        {
            auto *pPointLight = pointLights[i];
            auto move = mDirections[i] * deltaTime;
            auto position = pPointLight->GetGameObject()->GetTransform()->GetWorldPosition();
            pPointLight->GetGameObject()->GetTransform()->SetPosition(position + move);
        }
    }
}
