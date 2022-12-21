#include "Controls.h"

#include <FastCG/WorldSystem.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/MathT.h>
#include <FastCG/Key.h>
#include <FastCG/InputSystem.h>
#include <FastCG/Camera.h>
#include <FastCG/Application.h>

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

	auto *pMainCamera = WorldSystem::GetInstance()->GetMainCamera();
	auto *pWorldRenderer = WorldSystem::GetInstance()->GetWorldRenderer();

	if (ImGui::Begin("Controls"))
	{
		auto ssaoEnabled = pMainCamera->IsSSAOEnabled();
		ImGui::Checkbox("SSAO enabled (F1)", &ssaoEnabled);
		pMainCamera->SetSSAOEnabled(ssaoEnabled);
		auto ssaoBlurEnabled = pWorldRenderer->IsSSAOBlurEnabled();
		ImGui::Checkbox("SSAO blur enabled (F2)", &ssaoBlurEnabled);
		pWorldRenderer->SetSSAOBlurEnabled(ssaoBlurEnabled);
		auto ssaoRadius = pWorldRenderer->GetSSAORadius();
		ImGui::SliderFloat("SSAO radius (+/-)", &ssaoRadius, 0.01f, 0.1f);
		pWorldRenderer->SetSSAORadius(ssaoRadius);
		auto ssaoDistanceScale = pWorldRenderer->GetSSAODistanceScale();
		ImGui::SliderFloat("SSAO distance scale (Home/End)", &ssaoDistanceScale, 1, 100);
		pWorldRenderer->SetSSAODistanceScale(ssaoDistanceScale);
	}
	ImGui::End();

	InputSystem::IsKeyPressed(Key::F1, mPressedKeyMask, 0, [&]()
							  { pMainCamera->SetSSAOEnabled(!pMainCamera->IsSSAOEnabled()); });
	InputSystem::IsKeyPressed(Key::F2, mPressedKeyMask, 2, [&]()
							  { pWorldRenderer->SetSSAOBlurEnabled(!pWorldRenderer->IsSSAOBlurEnabled()); });
	InputSystem::IsKeyPressed(Key::PLUS, mPressedKeyMask, 3, [&]()
							  { pWorldRenderer->SetSSAORadius(MathF::Clamp(pWorldRenderer->GetSSAORadius() + 0.01f, 0.01f, 10)); });
	InputSystem::IsKeyPressed(Key::MINUS, mPressedKeyMask, 4, [&]()
							  { pWorldRenderer->SetSSAORadius(MathF::Clamp(pWorldRenderer->GetSSAORadius() - 0.01f, 0.01f, 10)); });
	InputSystem::IsKeyPressed(Key::HOME, mPressedKeyMask, 5, [&]()
							  { pWorldRenderer->SetSSAODistanceScale(MathF::Clamp(pWorldRenderer->GetSSAODistanceScale() + 5, 0.1f, 100)); });
	InputSystem::IsKeyPressed(Key::END, mPressedKeyMask, 6, [&]()
							  { pWorldRenderer->SetSSAODistanceScale(MathF::Clamp(pWorldRenderer->GetSSAODistanceScale() - 5, 0.1f, 100)); });
}
