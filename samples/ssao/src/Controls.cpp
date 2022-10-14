#include "Controls.h"

#include <FastCG/RenderingSystem.h>
#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLDeferredRenderingStrategy.h>
#endif
#include <FastCG/MathT.h>
#include <FastCG/KeyCode.h>
#include <FastCG/InputSystem.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/Application.h>

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
	const unsigned int text1Height = FontRegistry::STANDARD_FONT_SIZE + 3;
	const unsigned int text2Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 2 + 5;
	const unsigned int text3Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 3 + 7;
	const unsigned int text4Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 4 + 9;
	const unsigned int text5Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 5 + 11;
	const unsigned int text6Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 6 + 13;
	const unsigned int text7Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 7 + 15;
	const unsigned int text8Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 8 + 17;
	const unsigned int text9Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 9 + 18;

	RenderingSystem::GetInstance()->DrawDebugText("Press 'F1' to toggle SSAO", 10, text1Height, Colors::LIME);
	RenderingSystem::GetInstance()->DrawDebugText("Press 'F2' to display SSAO texture", 10, text2Height, Colors::LIME);
	RenderingSystem::GetInstance()->DrawDebugText("Press 'F3' to toggle SSAO blur", 10, text3Height, Colors::LIME);
	RenderingSystem::GetInstance()->DrawDebugText("Press +/- to change SSAO radius", 10, text4Height, Colors::LIME);
	RenderingSystem::GetInstance()->DrawDebugText("Press Home/End to change SSAO distance scale", 10, text5Height, Colors::LIME);

	auto *pMainCamera = BaseApplication::GetInstance()->GetMainCamera();

	auto pDeferredRenderingStrategy = static_cast<OpenGLDeferredRenderingStrategy *>(OpenGLRenderingSystem::GetInstance()->GetRenderingPathStrategy());

	float ssaoRadius = pDeferredRenderingStrategy->GetSSAORadius();
	float ssaoDistanceScale = pDeferredRenderingStrategy->GetSSAODistanceScale();

	char text[128];

	sprintf_s(text, FASTCG_ARRAYSIZE(text), "SSAO Radius: %.3f", ssaoRadius);
	RenderingSystem::GetInstance()->DrawDebugText(text, 10, text6Height, Colors::LIME);

	sprintf_s(text, FASTCG_ARRAYSIZE(text), "SSAO Distance Scale: %.3f", ssaoDistanceScale);
	RenderingSystem::GetInstance()->DrawDebugText(text, 10, text7Height, Colors::LIME);

	auto isSSAOEnabled = pMainCamera->IsSSAOEnabled();
	RenderingSystem::GetInstance()->DrawDebugText((isSSAOEnabled) ? "SSAO: on" : "SSAO: off", 10, text8Height, Colors::LIME);

	auto isSSAOBlurEnabled = pDeferredRenderingStrategy->IsSSAOBlurEnabled();
	RenderingSystem::GetInstance()->DrawDebugText((isSSAOBlurEnabled) ? "SSAO Blur: on" : "SSAO Blur: off", 10, text9Height, Colors::LIME);

	IsKeyPressed(KeyCode::F1, mPressedKeyMask, 0, [&]()
				 { pMainCamera->SetSSAOEnabled(!isSSAOEnabled); });
	IsKeyPressed(KeyCode::F2, mPressedKeyMask, 1, [&]()
				 { pDeferredRenderingStrategy->SetDisplaySSAOTextureEnabled(!pDeferredRenderingStrategy->IsDisplaySSAOTextureEnabled()); });
	IsKeyPressed(KeyCode::F3, mPressedKeyMask, 2, [&]()
				 { pDeferredRenderingStrategy->SetSSAOBlurEnabled(!isSSAOBlurEnabled); });
	IsKeyPressed(KeyCode::PLUS, mPressedKeyMask, 3, [&]()
				 { pDeferredRenderingStrategy->SetSSAORadius(MathF::Clamp(ssaoRadius + 1 * deltaTime, 0.01f, 10)); });
	IsKeyPressed(KeyCode::MINUS, mPressedKeyMask, 4, [&]()
				 { pDeferredRenderingStrategy->SetSSAORadius(MathF::Clamp(ssaoRadius - 1 * deltaTime, 0.01f, 10)); });
	IsKeyPressed(KeyCode::HOME, mPressedKeyMask, 5, [&]()
				 { pDeferredRenderingStrategy->SetSSAODistanceScale(MathF::Clamp(ssaoDistanceScale + 33 * deltaTime, 0.1f, 100)); });
	IsKeyPressed(KeyCode::END, mPressedKeyMask, 6, [&]()
				 { pDeferredRenderingStrategy->SetSSAODistanceScale(MathF::Clamp(ssaoDistanceScale - 33 * deltaTime, 0.1f, 100)); });
#endif
}
