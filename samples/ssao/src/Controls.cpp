#include "Controls.h"

#include <FastCG/RenderingSystem.h>
#include <FastCG/MathT.h>
#include <FastCG/Key.h>
#include <FastCG/InputSystem.h>
#include <FastCG/DeferredWorldRenderer.h>
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

	auto *pMainCamera = BaseApplication::GetInstance()->GetMainCamera();
	auto *pDeferredWorldRenderer = static_cast<DeferredWorldRenderer *>(Application::GetInstance()->GetWorldRenderer());

	if (ImGui::Begin("Controls"))
	{
		auto ssaoEnabled = pMainCamera->IsSSAOEnabled();
		ImGui::Checkbox("SSAO enabled (F1)", &ssaoEnabled);
		pMainCamera->SetSSAOEnabled(ssaoEnabled);
		auto ssaoBlurEnabled = pDeferredWorldRenderer->IsSSAOBlurEnabled();
		ImGui::Checkbox("SSAO blur enabled (F2)", &ssaoBlurEnabled);
		pDeferredWorldRenderer->SetSSAOBlurEnabled(ssaoBlurEnabled);
		auto ssaoRadius = pDeferredWorldRenderer->GetSSAORadius();
		ImGui::SliderFloat("SSAO radius (+/-)", &ssaoRadius, 0.01f, 0.1f);
		pDeferredWorldRenderer->SetSSAORadius(ssaoRadius);
		auto ssaoDistanceScale = pDeferredWorldRenderer->GetSSAODistanceScale();
		ImGui::SliderFloat("SSAO distance scale (Home/End)", &ssaoDistanceScale, 1, 100);
		pDeferredWorldRenderer->SetSSAODistanceScale(ssaoDistanceScale);
	}
	ImGui::End();

	InputSystem::IsKeyPressed(Key::F1, mPressedKeyMask, 0, [&]()
							  { pMainCamera->SetSSAOEnabled(!pMainCamera->IsSSAOEnabled()); });
	InputSystem::IsKeyPressed(Key::F2, mPressedKeyMask, 2, [&]()
							  { pDeferredWorldRenderer->SetSSAOBlurEnabled(!pDeferredWorldRenderer->IsSSAOBlurEnabled()); });
	InputSystem::IsKeyPressed(Key::PLUS, mPressedKeyMask, 3, [&]()
							  { pDeferredWorldRenderer->SetSSAORadius(MathF::Clamp(pDeferredWorldRenderer->GetSSAORadius() + 0.01f, 0.01f, 10)); });
	InputSystem::IsKeyPressed(Key::MINUS, mPressedKeyMask, 4, [&]()
							  { pDeferredWorldRenderer->SetSSAORadius(MathF::Clamp(pDeferredWorldRenderer->GetSSAORadius() - 0.01f, 0.01f, 10)); });
	InputSystem::IsKeyPressed(Key::HOME, mPressedKeyMask, 5, [&]()
							  { pDeferredWorldRenderer->SetSSAODistanceScale(MathF::Clamp(pDeferredWorldRenderer->GetSSAODistanceScale() + 5, 0.1f, 100)); });
	InputSystem::IsKeyPressed(Key::END, mPressedKeyMask, 6, [&]()
							  { pDeferredWorldRenderer->SetSSAODistanceScale(MathF::Clamp(pDeferredWorldRenderer->GetSSAODistanceScale() - 5, 0.1f, 100)); });
}
