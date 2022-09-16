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
	const unsigned int text1Height = FontRegistry::STANDARD_FONT_SIZE + 3;
	const unsigned int text2Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 2 + 5;
	const unsigned int text3Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 3 + 7;
	const unsigned int text4Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 4 + 9;
	const unsigned int text5Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 5 + 11;
	const unsigned int text6Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 6 + 13;
	const unsigned int text7Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 7 + 15;
	const unsigned int text8Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 8 + 17;

	Application::GetInstance()->DrawText("Press 'F1' to toggle SSAO", 10, text1Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Press 'F2' to display SSAO texture", 10, text2Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Press 'F3' to toggle SSAO blur", 10, text3Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Press +/- to change SSAO radius", 10, text4Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Press Home/End to change SSAO distance scale", 10, text5Height, glm::vec4{ 0, 1, 0, 1 });

	auto* pCamera = Application::GetInstance()->GetMainCamera();

	auto pDeferredRenderingStrategy = std::dynamic_pointer_cast<DeferredRenderingStrategy>(Application::GetInstance()->GetRenderingStrategy());
	assert(pDeferredRenderingStrategy != nullptr);

	float ssaoRadius = pDeferredRenderingStrategy->GetSSAORadius();
	float ssaoDistanceScale = pDeferredRenderingStrategy->GetSSAODistanceScale();

	char text[128];
	sprintf_s(text, sizeof(text) / sizeof(char), "SSAO Radius: %.3f", ssaoRadius);
	Application::GetInstance()->DrawText(text, 10, text6Height, glm::vec4{ 0, 1, 0, 1 });

	sprintf_s(text, sizeof(text) / sizeof(char), "SSAO Distance Scale: %.3f", ssaoDistanceScale);
	Application::GetInstance()->DrawText(text, 10, text7Height, glm::vec4{ 0, 1, 0, 1 });

	bool isSSAOEnabled = pCamera->IsSSAOEnabled();
	Application::GetInstance()->DrawText((isSSAOEnabled) ? "SSAO: on" : "SSAO: off", 10, text8Height, glm::vec4{ 0, 1, 0, 1 });

	if (Input::GetKey(KeyCode::F1) && time - mLastKeyPressTime > 0.333f)
	{
		pCamera->SetSSAOEnabled(!isSSAOEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::F2) && time - mLastKeyPressTime > 0.333f)
	{
		bool displaySSAOTextureEnabled = pDeferredRenderingStrategy->IsDisplaySSAOTextureEnabled();
		pDeferredRenderingStrategy->SetDisplaySSAOTextureEnabled(!displaySSAOTextureEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::F3) && time - mLastKeyPressTime > 0.333f)
	{
		bool ssaoBlurEnabled = pDeferredRenderingStrategy->IsSSAOBlurEnabled();
		pDeferredRenderingStrategy->SetSSAOBlurEnabled(!ssaoBlurEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::PLUS) && time - mLastKeyPressTime > 0.333f)
	{
		pDeferredRenderingStrategy->SetSSAORadius(MathF::Clamp(ssaoRadius + 1.0f * deltaTime, 0.01f, 1.0f));
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::MINUS) && time - mLastKeyPressTime > 0.333f)
	{
		pDeferredRenderingStrategy->SetSSAORadius(MathF::Clamp(ssaoRadius - 1.0f * deltaTime, 0.01f, 1.0f));
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::HOME) && time - mLastKeyPressTime > 0.333f)
	{
		pDeferredRenderingStrategy->SetSSAODistanceScale(MathF::Clamp(ssaoDistanceScale + 33.0f * deltaTime, 0.1f, 100.0f));
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::END) && time - mLastKeyPressTime > 0.333f)
	{
		pDeferredRenderingStrategy->SetSSAODistanceScale(MathF::Clamp(ssaoDistanceScale - 33.0f * deltaTime, 0.1f, 100.0f));
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
	}
}
