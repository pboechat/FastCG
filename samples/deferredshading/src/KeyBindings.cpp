#include "KeyBindings.h"

#include <Input.h>
#include <KeyCode.h>
#include <DeferredRenderingStrategy.h>

COMPONENT_IMPLEMENTATION(KeyBindings, Behaviour);

void KeyBindings::OnUpdate(float time, float deltaTime)
{
	if (Input::GetKey(KeyCode::F1) && time - mLastKeyPressTime > 0.333f)
	{
		DeferredRenderingStrategy* pDeferredRenderingStrategy = dynamic_cast<DeferredRenderingStrategy*>(Application::GetInstance()->GetRenderingStrategy());
		bool debugEnabled = pDeferredRenderingStrategy->IsDebugEnabled();
		pDeferredRenderingStrategy->SetDebugEnabled(!debugEnabled);
		mLastKeyPressTime = time;
	}
}
