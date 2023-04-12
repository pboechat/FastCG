#include "LightsAnimator.h"

#include <FastCG/World/WorldSystem.h>
#include <FastCG/World/Transform.h>
#include <FastCG/Rendering/Light.h>

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

	std::vector<Light *> lights;
	WorldSystem::GetInstance()->FindComponents<Light>(lights);
	for (auto *pLight : lights)
	{
		pLight->GetGameObject()->GetTransform()->RotateAroundLocal(deltaTime * 100, glm::vec3(0, 1, 0));
	}
}
