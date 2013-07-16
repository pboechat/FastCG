#include "KeyBindings.h"

#include <Application.h>
#include <FontRegistry.h>
#include <Input.h>
#include <KeyCode.h>
#include <DeferredRenderingStrategy.h>
#include <MathT.h>

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

	Application::GetInstance()->DrawText("Press 'F1' to toggle SSAO", FontRegistry::STANDARD_FONT_SIZE, 10, text1Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press 'F2' to display SSAO texture", FontRegistry::STANDARD_FONT_SIZE, 10, text2Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press +/- to change SSAO ray length", FontRegistry::STANDARD_FONT_SIZE, 10, text3Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press Home/End to change SSAO exponent", FontRegistry::STANDARD_FONT_SIZE, 10, text4Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	Camera* pMainCamera = Application::GetInstance()->GetMainCamera();

	DeferredRenderingStrategy* pDeferredRenderingStrategy = dynamic_cast<DeferredRenderingStrategy*>(Application::GetInstance()->GetRenderingStrategy());
	float ssaoRayLength = pDeferredRenderingStrategy->GetSSAORayLength();
	unsigned int ssaoExponent = pDeferredRenderingStrategy->GetSSAOExponent();

	sprintf(pText, "SSAO Ray Length: %.3f", ssaoRayLength);
	Application::GetInstance()->DrawText(pText, FontRegistry::STANDARD_FONT_SIZE, 10, text5Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	sprintf(pText, "SSAO Exponent: %d", ssaoExponent);
	Application::GetInstance()->DrawText(pText, FontRegistry::STANDARD_FONT_SIZE, 10, text6Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	bool isSSAOEnabled = pMainCamera->IsSSAOEnabled();
	Application::GetInstance()->DrawText((isSSAOEnabled) ? "SSAO: on" : "SSAO: off", FontRegistry::STANDARD_FONT_SIZE, 10, text7Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	if (Input::GetKey(KeyCode::F1) && time - mLastKeyPressTime > 0.333f)
	{
		pMainCamera->SetSSAOEnabled(!isSSAOEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::F2) && time - mLastKeyPressTime > 0.333f)
	{
		bool displaySSAOTextureEnabled = pDeferredRenderingStrategy->IsDisplaySSAOTextureEnabled();
		pDeferredRenderingStrategy->SetDisplaySSAOTextureEnabled(!displaySSAOTextureEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::PLUS) && time - mLastKeyPressTime > 0.333f)
	{
		pDeferredRenderingStrategy->SetSSAORayLength(MathF::Max(ssaoRayLength + 100.0f * deltaTime, 1.0f));
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::MINUS) && time - mLastKeyPressTime > 0.333f)
	{
		pDeferredRenderingStrategy->SetSSAORayLength(MathF::Max(ssaoRayLength - 100.0f * deltaTime, 1.0f));
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::HOME) && time - mLastKeyPressTime > 0.333f)
	{
		pDeferredRenderingStrategy->SetSSAOExponent(MathUI::Max(ssaoExponent + 1, 1));
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::END) && time - mLastKeyPressTime > 0.333f)
	{
		pDeferredRenderingStrategy->SetSSAOExponent(MathUI::Max(ssaoExponent - 1, 1));
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
	}
}
