#include "Controls.h"

#include <FastCG/RenderingSystem.h>
#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLDeferredRenderingPathStrategy.h>
#endif
#include <FastCG/MathT.h>
#include <FastCG/Key.h>
#include <FastCG/InputSystem.h>
#include <FastCG/Application.h>

using namespace FastCG;

IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
	if (InputSystem::GetKey(Key::ESCAPE))
	{
		Application::GetInstance()->Exit();
		return;
	}

#ifdef FASTCG_OPENGL
	auto *pMainCamera = BaseApplication::GetInstance()->GetMainCamera();
	auto *pDeferredRenderingStrategy = static_cast<OpenGLDeferredRenderingPathStrategy *>(OpenGLRenderingSystem::GetInstance()->GetRenderingPathStrategy());

#if 0
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

	char text[128];

	sprintf_s(text, FASTCG_ARRAYSIZE(text), "SSAO Radius: %.3f", ssaoRadius);
	RenderingSystem::GetInstance()->DrawDebugText(text, 10, text6Height, Colors::LIME);

	sprintf_s(text, FASTCG_ARRAYSIZE(text), "SSAO Distance Scale: %.3f", ssaoDistanceScale);
	RenderingSystem::GetInstance()->DrawDebugText(text, 10, text7Height, Colors::LIME);

	RenderingSystem::GetInstance()->DrawDebugText((isSSAOEnabled) ? "SSAO: on" : "SSAO: off", 10, text8Height, Colors::LIME);
	RenderingSystem::GetInstance()->DrawDebugText((isSSAOBlurEnabled) ? "SSAO Blur: on" : "SSAO Blur: off", 10, text9Height, Colors::LIME);
#endif

	InputSystem::IsKeyPressed(Key::F1, mPressedKeyMask, 0, [&]()
							  { pMainCamera->SetSSAOEnabled(!pMainCamera->IsSSAOEnabled()); });
#ifdef _DEBUG
	InputSystem::IsKeyPressed(Key::F2, mPressedKeyMask, 1, [&]()
							  { pDeferredRenderingStrategy->SetDisplaySSAOTextureEnabled(!pDeferredRenderingStrategy->IsDisplaySSAOTextureEnabled()); });
#endif
	InputSystem::IsKeyPressed(Key::F3, mPressedKeyMask, 2, [&]()
							  { pDeferredRenderingStrategy->SetSSAOBlurEnabled(!pDeferredRenderingStrategy->IsSSAOBlurEnabled()); });
	InputSystem::IsKeyPressed(Key::PLUS, mPressedKeyMask, 3, [&]()
							  { pDeferredRenderingStrategy->SetSSAORadius(MathF::Clamp(pDeferredRenderingStrategy->GetSSAORadius() + 1 * deltaTime, 0.01f, 10)); });
	InputSystem::IsKeyPressed(Key::MINUS, mPressedKeyMask, 4, [&]()
							  { pDeferredRenderingStrategy->SetSSAORadius(MathF::Clamp(pDeferredRenderingStrategy->GetSSAORadius() - 1 * deltaTime, 0.01f, 10)); });
	InputSystem::IsKeyPressed(Key::HOME, mPressedKeyMask, 5, [&]()
							  { pDeferredRenderingStrategy->SetSSAODistanceScale(MathF::Clamp(pDeferredRenderingStrategy->GetSSAODistanceScale() + 33 * deltaTime, 0.1f, 100)); });
	InputSystem::IsKeyPressed(Key::END, mPressedKeyMask, 6, [&]()
							  { pDeferredRenderingStrategy->SetSSAODistanceScale(MathF::Clamp(pDeferredRenderingStrategy->GetSSAODistanceScale() - 33 * deltaTime, 0.1f, 100)); });
#endif
}
