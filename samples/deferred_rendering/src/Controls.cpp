#include "Controls.h"

#if defined _DEBUG && defined FASTCG_OPENGL
#include <FastCG/OpenGLDeferredRenderingPathStrategy.h>
#endif
#include <FastCG/KeyCode.h>
#include <FastCG/InputSystem.h>
#include <FastCG/Application.h>

using namespace FastCG;

IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
	if (InputSystem::GetKey(KeyCodes::ESCAPE))
	{
		Application::GetInstance()->Exit();
		return;
	}

#if defined _DEBUG && defined FASTCG_OPENGL
	auto *pDeferredRenderingStrategy = static_cast<OpenGLDeferredRenderingPathStrategy *>(RenderingSystem::GetInstance()->GetRenderingPathStrategy());
	InputSystem::IsKeyPressed(KeyCodes::F1, mPressedKeyMask, 0, [&]()
							  { pDeferredRenderingStrategy->SetDisplayGBufferEnabled(!pDeferredRenderingStrategy->IsDisplayGBufferEnabled()); });
	InputSystem::IsKeyPressed(KeyCodes::F2, mPressedKeyMask, 1, [&]()
							  { pDeferredRenderingStrategy->SetShowPointLightsEnabled(!pDeferredRenderingStrategy->IsShowPointLightsEnabled()); });
#endif
}
