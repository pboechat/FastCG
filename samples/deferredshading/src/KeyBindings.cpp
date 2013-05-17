#include "KeyBindings.h"

#include <Input.h>
#include <KeyCode.h>
#include <DeferredRenderingStrategy.h>

COMPONENT_IMPLEMENTATION(KeyBindings, Behaviour);

void KeyBindings::OnUpdate(float time, float deltaTime)
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	if (Input::GetKey(KeyCode::F1) && time - mLastKeyPressTime > 0.333f)
	{
		DeferredRenderingStrategy* pDeferredRenderingStrategy = dynamic_cast<DeferredRenderingStrategy*>(Application::GetInstance()->GetRenderingStrategy());
		bool displayGBufferEnabled = pDeferredRenderingStrategy->IsDisplayGBufferEnabled();
		pDeferredRenderingStrategy->SetDisplayGBufferEnabled(!displayGBufferEnabled);
		mLastKeyPressTime = time;
	}
	else if (Input::GetKey(KeyCode::F2) && time - mLastKeyPressTime > 0.333f)
	{
		DeferredRenderingStrategy* pDeferredRenderingStrategy = dynamic_cast<DeferredRenderingStrategy*>(Application::GetInstance()->GetRenderingStrategy());
		bool showPointLightsEnabled = pDeferredRenderingStrategy->IsShowPointLightsEnabled();
		pDeferredRenderingStrategy->SetShowPointLightsEnabled(!showPointLightsEnabled);
		mLastKeyPressTime = time;
	}
#endif
}
