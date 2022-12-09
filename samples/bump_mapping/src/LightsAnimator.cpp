#include "LightsAnimator.h"

#include <FastCG/Key.h>
#include <FastCG/Transform.h>
#include <FastCG/InputSystem.h>
#include <FastCG/Application.h>

#include <glm/glm.hpp>

using namespace FastCG;

FASTCG_IMPLEMENT_COMPONENT(LightsAnimator, Behaviour);

void LightsAnimator::OnUpdate(float time, float deltaTime)
{
	if (InputSystem::GetKey(Key::ESCAPE))
	{
		Application::GetInstance()->Exit();
		return;
	}

	for (auto *pSceneLight : mSceneLights)
	{
		pSceneLight->GetTransform()->RotateAroundLocal(deltaTime * 100, glm::vec3(0, 1, 0));
	}
}
