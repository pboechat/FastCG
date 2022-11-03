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

	ImGui::Begin("Controls");
	ImGui::Text("Press 'F1' to toggle SSAO (%s)", pMainCamera->IsSSAOEnabled() ? "on" : "off");
	ImGui::Text("Press 'F2' to toggle SSAO blur (%s)", pDeferredWorldRenderer->IsSSAOBlurEnabled() ? "on" : "off");
	ImGui::Text("Press +/- to change SSAO radius (%f)", pDeferredWorldRenderer->GetSSAORadius());
	ImGui::Text("Press Home/End to change SSAO distance scale (%f)", pDeferredWorldRenderer->GetSSAODistanceScale());
	ImGui::End();

	InputSystem::IsKeyPressed(Key::F1, mPressedKeyMask, 0, [&]()
							  { pMainCamera->SetSSAOEnabled(!pMainCamera->IsSSAOEnabled()); });
	InputSystem::IsKeyPressed(Key::F2, mPressedKeyMask, 2, [&]()
							  { pDeferredWorldRenderer->SetSSAOBlurEnabled(!pDeferredWorldRenderer->IsSSAOBlurEnabled()); });
	InputSystem::IsKeyPressed(Key::PLUS, mPressedKeyMask, 3, [&]()
							  { pDeferredWorldRenderer->SetSSAORadius(MathF::Clamp(pDeferredWorldRenderer->GetSSAORadius() + 1 * deltaTime, 0.01f, 10)); });
	InputSystem::IsKeyPressed(Key::MINUS, mPressedKeyMask, 4, [&]()
							  { pDeferredWorldRenderer->SetSSAORadius(MathF::Clamp(pDeferredWorldRenderer->GetSSAORadius() - 1 * deltaTime, 0.01f, 10)); });
	InputSystem::IsKeyPressed(Key::HOME, mPressedKeyMask, 5, [&]()
							  { pDeferredWorldRenderer->SetSSAODistanceScale(MathF::Clamp(pDeferredWorldRenderer->GetSSAODistanceScale() + 33 * deltaTime, 0.1f, 100)); });
	InputSystem::IsKeyPressed(Key::END, mPressedKeyMask, 6, [&]()
							  { pDeferredWorldRenderer->SetSSAODistanceScale(MathF::Clamp(pDeferredWorldRenderer->GetSSAODistanceScale() - 33 * deltaTime, 0.1f, 100)); });
}
