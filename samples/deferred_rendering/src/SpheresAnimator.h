#ifndef SPHERES_ANIMATOR_H
#define SPHERES_ANIMATOR_H

#include <FastCG/Behaviour.h>

#include <glm/glm.hpp>

class SpheresAnimator : public FastCG::Behaviour
{
	DECLARE_COMPONENT(SpheresAnimator, FastCG::Behaviour);

public:
	inline void SetSpheres(std::vector<FastCG::GameObject *> spheres)
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

protected:
	void OnUpdate(float time, float deltaTime) override;

private:
	std::vector<FastCG::GameObject *> mSpheres;
	glm::vec3 mRotationAxis{0, 0, 0};
	float mRotationSpeed{0};
};

#endif