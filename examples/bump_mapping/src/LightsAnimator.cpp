#include "LightsAnimator.h"

#include <FastCG/Rendering/PointLight.h>
#include <FastCG/World/Transform.h>
#include <FastCG/World/WorldSystem.h>

#include <glm/glm.hpp>

#include <vector>

using namespace FastCG;

FASTCG_IMPLEMENT_COMPONENT(LightsAnimator, Behaviour);

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
    if (!mEnabled)
    {
        return;
    }

    std::vector<PointLight *> pointLights;
    WorldSystem::GetInstance()->FindComponents<PointLight>(pointLights);
    for (auto *pPointLight : pointLights)
    {
        pPointLight->GetGameObject()->GetTransform()->RotateAroundLocal(deltaTime * 100, glm::vec3(0, 1, 0));
    }
}
