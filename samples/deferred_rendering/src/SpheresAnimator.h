#ifndef SPHERES_ANIMATOR_H_
#define SPHERES_ANIMATOR_H_

#include <FastCG/Behaviour.h>
#include <FastCG/Texture.h>

#include <glm/glm.hpp>

using namespace FastCG;

class SpheresAnimator : public Behaviour
{
	DECLARE_COMPONENT(SpheresAnimator, Behaviour);

public:
	void OnInstantiate() override
	{
		mRotationSpeed = 0.0f;
		mRotationAxis = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	inline void SetSpheres(std::vector<GameObject *> spheres)
	{
		mSpheres = spheres;
	}

	inline void SetRotationSpeed(float rotationSpeed)
	{
		mRotationSpeed = rotationSpeed;
	}

	inline void SetRotationAxis(const glm::vec3 &rotationAxis)
	{
		mRotationAxis = rotationAxis;
	}

	void OnUpdate(float time, float deltaTime) override;

private:
	std::vector<GameObject *> mSpheres;
	glm::vec3 mRotationAxis;
	float mRotationSpeed;
};

#endif