#include "Controls.h"

#include <FastCG/Key.h>
#include <FastCG/InputSystem.h>
#include <FastCG/Application.h>

#include <imgui.h>

using namespace FastCG;

IMPLEMENT_COMPONENT(Controls, Behaviour);

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
		ImGui::End();
	}
}
