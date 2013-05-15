#ifndef KEYBINDINGS_H_
#define KEYBINDINGS_H_

#include <Behaviour.h>

COMPONENT(KeyBindings, Behaviour)
public:
	virtual void OnUpdate(float time, float deltaTime);

	virtual void OnInstantiate()
	{
		mLastKeyPressTime = 0;
	}

private:
	float mLastKeyPressTime;

};

#endif