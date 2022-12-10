#include "LightsAnimator.h"

#include <FastCG/Transform.h>

#include <glm/glm.hpp>

using namespace FastCG;

FASTCG_IMPLEMENT_COMPONENT(LightsAnimator, Behaviour);

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
	if (!mEnabled)
	{
		return;
	}

	for (auto *pSceneLight : mSceneLights)
	{
		pSceneLight->GetTransform()->RotateAroundLocal(deltaTime * 100, glm::vec3(0, 1, 0));
	}
}
