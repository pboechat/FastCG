#ifndef KEYBINDINGS_H_
#define KEYBINDINGS_H_

#include <Behaviour.h>
#include <Material.h>

#include <vector>

COMPONENT(KeyBindings, Behaviour)
public:
	inline void SetModelMaterials(const std::vector<Material*>& rMaterials)
	{
		mMaterials = rMaterials;
	}

	inline void SetSceneLights(const std::vector<GameObject*>& rSceneLights)
	{
		mSceneLights = rSceneLights;
	}

protected:
	virtual void OnUpdate(float time, float deltaTime);

	virtual void OnInstantiate()
	{
		mLastKeyPressTime = 0;
	}

private:
	float mLastKeyPressTime;
	std::vector<GameObject*> mSceneLights;
	std::vector<Material*> mMaterials;

};

#endif