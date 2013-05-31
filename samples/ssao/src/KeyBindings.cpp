#include "KeyBindings.h"

#include <Application.h>
#include <FontRegistry.h>
#include <Input.h>
#include <KeyCode.h>
#include <DeferredRenderingStrategy.h>

COMPONENT_IMPLEMENTATION(KeyBindings, Behaviour);

void KeyBindings::OnUpdate(float time, float deltaTime)
{
#ifdef FIXED_FUNCTION_PIPELINE
	static unsigned int text1Height = 15;
	Application::GetInstance()->DrawText("Use WASD/arrow keys and left mouse button to navigate", 12, 10, text1Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
#else
	static unsigned int text1Height = FontRegistry::STANDARD_FONT_SIZE + 3;
	static unsigned int text2Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 2 + 5;
	static unsigned int text3Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 3 + 5;
	Application::GetInstance()->DrawText("Use WASD/arrow keys and left mouse button to navigate", FontRegistry::STANDARD_FONT_SIZE, 10, text1Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press 'F1' to toggle SSAO", FontRegistry::STANDARD_FONT_SIZE, 10, text2Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press 'F2' to display SSAO texture", FontRegistry::STANDARD_FONT_SIZE, 10, text3Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
#endif
	if (Input::GetKey(KeyCode::F1) && time - mLastKeyPressTime > 0.333f)
	{
		Camera* pMainCamera = Application::GetInstance()->GetMainCamera();
		bool isSSAOEnabled = pMainCamera->IsSSAOEnabled();
		pMainCamera->SetSSAOEnabled(!isSSAOEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::F2) && time - mLastKeyPressTime > 0.333f)
	{
		DeferredRenderingStrategy* pDeferredRenderingStrategy = dynamic_cast<DeferredRenderingStrategy*>(Application::GetInstance()->GetRenderingStrategy());
		bool displaySSAOTextureEnabled = pDeferredRenderingStrategy->IsDisplaySSAOTextureEnabled();
		pDeferredRenderingStrategy->SetDisplaySSAOTextureEnabled(!displaySSAOTextureEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
	}
}
