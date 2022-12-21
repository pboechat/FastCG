#include "LightsAnimator.h"

#include <FastCG/WorldSystem.h>
#include <FastCG/Transform.h>
#include <FastCG/Light.h>

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

	std::vector<Light *> mLights;
	WorldSystem::GetInstance()->FindComponents<Light>(mLights);
	for (auto *pLight : mLights)
	{
		pLight->GetGameObject()->GetTransform()->RotateAroundLocal(deltaTime * 100, glm::vec3(0, 1, 0));
	}
}
