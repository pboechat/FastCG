#include "Controls.h"

#include <FastCG/InputSystem.h>
#include <FastCG/Application.h>

using namespace FastCG;

IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
    if (InputSystem::GetKey(Key::ESCAPE))
    {
        Application::GetInstance()->Exit();
        return;
    }
}
