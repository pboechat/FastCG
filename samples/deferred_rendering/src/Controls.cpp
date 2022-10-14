#include "Controls.h"

#include <FastCG/RenderingSystem.h>
#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLDeferredRenderingStrategy.h>
#endif
#include <FastCG/KeyCode.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/InputSystem.h>
#include <FastCG/Application.h>

#include <memory>
#include <cstdint>
#include <cassert>

IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
	if (InputSystem::GetKey(KeyCode::ESCAPE))
	{
		BaseApplication::GetInstance()->Exit();
		return;
	}

#ifdef FASTCG_OPENGL
	const uint32_t text1Height = FontRegistry::STANDARD_FONT_SIZE + 3;
	const uint32_t text2Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 2 + 5;
	const uint32_t text3Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 3 + 7;

	RenderingSystem::GetInstance()->DrawDebugText("Press 'F1' to toggle G-Buffer display", 10, text1Height, Colors::LIME);
	RenderingSystem::GetInstance()->DrawDebugText("Press 'F2' to toggle point lights display", 10, text2Height, Colors::LIME);
	RenderingSystem::GetInstance()->DrawDebugText("Use WASD/arrow keys and left mouse button to navigate", 10, text3Height, Colors::LIME);

	auto *pDeferredRenderingStrategy = static_cast<OpenGLDeferredRenderingStrategy *>(RenderingSystem::GetInstance()->GetRenderingPathStrategy());
	IsKeyPressed(KeyCode::F1, mPressedKeyMask, 0, [&]()
				 {
			assert(pDeferredRenderingStrategy != nullptr);
			auto displayGBufferEnabled = pDeferredRenderingStrategy->IsDisplayGBufferEnabled();
			pDeferredRenderingStrategy->SetDisplayGBufferEnabled(!displayGBufferEnabled); });
	IsKeyPressed(KeyCode::F2, mPressedKeyMask, 1, [&]()
				 {
			assert(pDeferredRenderingStrategy != nullptr);
			auto showPointLightsEnabled = pDeferredRenderingStrategy->IsShowPointLightsEnabled();
			pDeferredRenderingStrategy->SetShowPointLightsEnabled(!showPointLightsEnabled); });
#endif
}
