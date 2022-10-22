#include "Controls.h"

#include <FastCG/Transform.h>
#include <FastCG/MathT.h>
#include <FastCG/KeyCode.h>
#include <FastCG/InputSystem.h>
#include <FastCG/Application.h>

IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
	if (InputSystem::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
		return;
	}
}
