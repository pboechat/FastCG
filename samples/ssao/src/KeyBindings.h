#ifndef KEYBINDINGS_H_
#define KEYBINDINGS_H_

#include <Behaviour.h>
#include <DirectionalLight.h>

COMPONENT(KeyBindings, Behaviour)
public:
	inline void SetSceneLights(GameObject* pSceneLights)
	{
		mpSceneLights = pSceneLights;
	}

	inline void SetLightDistance(float lightDistance)
	{
		mLightDistance = lightDistance;
	}

protected:
	virtual void OnUpdate(float time, float deltaTime);

	virtual void OnInstantiate()
	{
		mLastKeyPressTime = 0;
		mpSceneLights = 0;
		mLightDistance = 1.0f;
	}

private:
	float mLastKeyPressTime;
	GameObject* mpSceneLights;
	float mLightDistance;

};

#endif