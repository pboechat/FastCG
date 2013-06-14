#ifndef KEYBINDINGS_H_
#define KEYBINDINGS_H_

#include <Behaviour.h>

#include <vector>

COMPONENT(KeyBindings, Behaviour)
public:
	inline void SetLightDistance(float lightDistance)
	{
		mLightDistance = lightDistance;
	}

protected:
	virtual void OnUpdate(float time, float deltaTime);

	virtual void OnInstantiate()
	{
		mLastKeyPressTime = 0;
		mLightDistance = 1.0f;
	}

private:
	float mLastKeyPressTime;
	float mLightDistance;

};

#endif