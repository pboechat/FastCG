#ifndef KEYBINDINGS_H_
#define KEYBINDINGS_H_

#include <Behaviour.h>

COMPONENT(KeyBindings, Behaviour)
public:
	virtual void OnUpdate(float time, float deltaTime);

	virtual void OnInstantiate()
	{
		mLastKeyPressTime = 0;
		mCurrentModelIndex = 0;
	}

	inline void SetModels(const std::vector<GameObject*>& rModels)
	{
		mModels = rModels;
	}

private:
	float mLastKeyPressTime;
	unsigned int mCurrentModelIndex;
	std::vector<GameObject*> mModels;

};

#endif