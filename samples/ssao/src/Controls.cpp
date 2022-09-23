#include "Controls.h"

#include <FastCG/Application.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/Input.h>
#include <FastCG/KeyCode.h>
#include <FastCG/DeferredRenderingStrategy.h>
#include <FastCG/MathT.h>

#include <cassert>

IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
	if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
		return;
	}

	const unsigned int text1Height = FontRegistry::STANDARD_FONT_SIZE + 3;
	const unsigned int text2Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 2 + 5;
	const unsigned int text3Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 3 + 7;
	const unsigned int text4Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 4 + 9;
	const unsigned int text5Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 5 + 11;
	const unsigned int text6Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 6 + 13;
	const unsigned int text7Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 7 + 15;
	const unsigned int text8Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 8 + 17;
	const unsigned int text9Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 9 + 18;

	Application::GetInstance()->DrawText("Press 'F1' to toggle SSAO", 10, text1Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Press 'F2' to display SSAO texture", 10, text2Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Press 'F3' to toggle SSAO blur", 10, text3Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Press +/- to change SSAO radius", 10, text4Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Press Home/End to change SSAO distance scale", 10, text5Height, glm::vec4{ 0, 1, 0, 1 });

	auto* pCamera = Application::GetInstance()->GetMainCamera();

	auto pDeferredRenderingStrategy = std::static_pointer_cast<DeferredRenderingStrategy>(Application::GetInstance()->GetRenderingStrategy());

	float ssaoRadius = pDeferredRenderingStrategy->GetSSAORadius();
	float ssaoDistanceScale = pDeferredRenderingStrategy->GetSSAODistanceScale();

	char text[128];
	sprintf_s(text, sizeof(text) / sizeof(char), "SSAO Radius: %.3f", ssaoRadius);
	Application::GetInstance()->DrawText(text, 10, text6Height, glm::vec4{ 0, 1, 0, 1 });

	sprintf_s(text, sizeof(text) / sizeof(char), "SSAO Distance Scale: %.3f", ssaoDistanceScale);
	Application::GetInstance()->DrawText(text, 10, text7Height, glm::vec4{ 0, 1, 0, 1 });

	auto isSSAOEnabled = pCamera->IsSSAOEnabled();
	Application::GetInstance()->DrawText((isSSAOEnabled) ? "SSAO: on" : "SSAO: off", 10, text8Height, glm::vec4{ 0, 1, 0, 1 });

	auto isSSAOBlurEnabled = pDeferredRenderingStrategy->IsSSAOBlurEnabled();
	Application::GetInstance()->DrawText((isSSAOBlurEnabled) ? "SSAO Blur: on" : "SSAO Blur: off", 10, text9Height, glm::vec4{ 0, 1, 0, 1 });

	IsKeyPressed(KeyCode::F1, mPressedKeyMask, 0, [&]() { pCamera->SetSSAOEnabled(!isSSAOEnabled); });
	IsKeyPressed(KeyCode::F2, mPressedKeyMask, 1, [&]() { pDeferredRenderingStrategy->SetDisplaySSAOTextureEnabled(!pDeferredRenderingStrategy->IsDisplaySSAOTextureEnabled()); });
	IsKeyPressed(KeyCode::F3, mPressedKeyMask, 2, [&]() { pDeferredRenderingStrategy->SetSSAOBlurEnabled(!isSSAOBlurEnabled); });
	IsKeyPressed(KeyCode::PLUS, mPressedKeyMask, 3, [&]() { pDeferredRenderingStrategy->SetSSAORadius(MathF::Clamp(ssaoRadius + 1 * deltaTime, 0.01f, 10)); });
	IsKeyPressed(KeyCode::MINUS, mPressedKeyMask, 4, [&]() { pDeferredRenderingStrategy->SetSSAORadius(MathF::Clamp(ssaoRadius - 1 * deltaTime, 0.01f, 10)); });
	IsKeyPressed(KeyCode::HOME, mPressedKeyMask, 5, [&]() { pDeferredRenderingStrategy->SetSSAODistanceScale(MathF::Clamp(ssaoDistanceScale + 33 * deltaTime, 0.1f, 100)); });
	IsKeyPressed(KeyCode::END, mPressedKeyMask, 6, [&]() { pDeferredRenderingStrategy->SetSSAODistanceScale(MathF::Clamp(ssaoDistanceScale - 33 * deltaTime, 0.1f, 100)); });
}
