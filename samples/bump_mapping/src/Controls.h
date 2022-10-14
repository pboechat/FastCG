#ifndef CONTROLS_H
#define CONTROLS_H

#include <FastCG/Material.h>
#include <FastCG/Behaviour.h>

#include <memory>

using namespace FastCG;

class Controls : public Behaviour
{
	DECLARE_COMPONENT(Controls, Behaviour);

public:
	inline void SetModelMaterials(const std::vector<std::shared_ptr<Material>> &rMaterials)
	{
		mMaterials = rMaterials;
	}

	inline void SetSceneLights(const std::vector<GameObject *> &rSceneLights)
	{
		mSceneLights = rSceneLights;
	}

protected:
	void OnUpdate(float time, float deltaTime) override;

private:
	std::vector<GameObject *> mSceneLights;
	std::vector<std::shared_ptr<Material>> mMaterials;
	uint8_t mPressedKeyMask{0};
};

#endif