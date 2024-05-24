#include "Controls.h"

#include <FastCG/Core/Math.h>
#include <FastCG/Input/InputSystem.h>
#include <FastCG/Input/Key.h>
#include <FastCG/Platform/Application.h>
#include <FastCG/World/Transform.h>

FASTCG_IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
    if (InputSystem::GetKey(Key::ESCAPE))
    {
        Application::GetInstance()->Exit();
        return;
    }
}
