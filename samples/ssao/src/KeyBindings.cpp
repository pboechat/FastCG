#include "KeyBindings.h"

#include <Application.h>
#include <FontRegistry.h>
#include <Input.h>
#include <KeyCode.h>
#include <DeferredRenderingStrategy.h>
#include <MathT.h>

#include <iostream>

COMPONENT_IMPLEMENTATION(KeyBindings, Behaviour);

void KeyBindings::OnUpdate(float time, float deltaTime)
{
	static char pText[128];

	static unsigned int text1Height = FontRegistry::STANDARD_FONT_SIZE + 3;
	static unsigned int text2Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 2 + 5;
	static unsigned int text3Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 3 + 7;
	static unsigned int text4Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 4 + 9;
	static unsigned int text5Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 5 + 11;
	static unsigned int text6Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 6 + 13;
	static unsigned int text7Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 7 + 15;
	static unsigned int text8Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 8 + 17;

	Application::GetInstance()->DrawText("Press 'F1' to toggle SSAO", FontRegistry::STANDARD_FONT_SIZE, 10, text1Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press 'F2' to display SSAO texture", FontRegistry::STANDARD_FONT_SIZE, 10, text2Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press 'F3' to toggle SSAO blur", FontRegistry::STANDARD_FONT_SIZE, 10, text3Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press +/- to change SSAO radius", FontRegistry::STANDARD_FONT_SIZE, 10, text4Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press Home/End to change SSAO distance scale", FontRegistry::STANDARD_FONT_SIZE, 10, text5Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	Camera* pCamera = Application::GetInstance()->GetMainCamera();

	DeferredRenderingStrategy* pDeferredRenderingStrategy = dynamic_cast<DeferredRenderingStrategy*>(Application::GetInstance()->GetRenderingStrategy());
	float ssaoRadius = pDeferredRenderingStrategy->GetSSAORadius();
	float ssaoDistanceScale = pDeferredRenderingStrategy->GetSSAODistanceScale();

	sprintf(pText, "SSAO Radius: %.3f", ssaoRadius);
	Application::GetInstance()->DrawText(pText, FontRegistry::STANDARD_FONT_SIZE, 10, text6Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	sprintf(pText, "SSAO Distance Scale: %.3f", ssaoDistanceScale);
	Application::GetInstance()->DrawText(pText, FontRegistry::STANDARD_FONT_SIZE, 10, text7Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	bool isSSAOEnabled = pCamera->IsSSAOEnabled();
	Application::GetInstance()->DrawText((isSSAOEnabled) ? "SSAO: on" : "SSAO: off", FontRegistry::STANDARD_FONT_SIZE, 10, text8Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

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
	/*else if (Input::GetKey(KeyCode::LETTER_C) && time - mLastKeyPressTime > 0.333f)
	{
		glm::vec3& rCameraPosition = pCamera->GetGameObject()->GetTransform()->GetPosition();
		std::cout << "Camera Position: (" << rCameraPosition.x << ", " << rCameraPosition.y << ", " << rCameraPosition.z << ")" << std::endl;
	}*/
	else if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
	}
}
