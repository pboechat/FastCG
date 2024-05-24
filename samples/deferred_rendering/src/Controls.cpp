#include "Controls.h"

#include <FastCG/Input/InputSystem.h>
#include <FastCG/Input/Key.h>
#include <FastCG/Platform/Application.h>

#include <imgui.h>

using namespace FastCG;

FASTCG_IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
    if (InputSystem::GetKey(Key::ESCAPE))
    {
        Application::GetInstance()->Exit();
        return;
    }

    if (ImGui::Begin("Controls"))
    {
        if (ImGui::Button("Change Colors"))
        {
            mpLightsAnimator->ChangeColors();
        }
    }
    ImGui::End();
}
