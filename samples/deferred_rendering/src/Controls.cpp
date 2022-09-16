#include "Controls.h"

#include <FastCG/DeferredRenderingStrategy.h>
#include <FastCG/KeyCode.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/Input.h>
#include <FastCG/Application.h>

#include <cstdint>
#include <cassert>

IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
	const uint32_t text1Height = FontRegistry::STANDARD_FONT_SIZE + 3;
	const uint32_t text2Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 2 + 5;
	const uint32_t text3Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 3 + 7;

	Application::GetInstance()->DrawText("Press 'F1' to toggle G-Buffer display", 10, text1Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Press 'F2' to toggle point lights display", 10, text2Height, glm::vec4{ 0, 1, 0, 1 });
	Application::GetInstance()->DrawText("Use WASD/arrow keys and left mouse button to navigate", 10, text3Height, glm::vec4{ 0, 1, 0, 1 });

	if (Input::GetKey(KeyCode::F1) && time - mLastKeyPressTime > 0.333f)
	{
		auto pDeferredRenderingStrategy = std::dynamic_pointer_cast<DeferredRenderingStrategy>(Application::GetInstance()->GetRenderingStrategy());
		assert(pDeferredRenderingStrategy != nullptr);
		auto displayGBufferEnabled = pDeferredRenderingStrategy->IsDisplayGBufferEnabled();
		pDeferredRenderingStrategy->SetDisplayGBufferEnabled(!displayGBufferEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::F2) && time - mLastKeyPressTime > 0.333f)
	{
		auto pDeferredRenderingStrategy = std::dynamic_pointer_cast<DeferredRenderingStrategy>(Application::GetInstance()->GetRenderingStrategy());
		assert(pDeferredRenderingStrategy != nullptr);
		auto showPointLightsEnabled = pDeferredRenderingStrategy->IsShowPointLightsEnabled();
		pDeferredRenderingStrategy->SetShowPointLightsEnabled(!showPointLightsEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
	}
}
