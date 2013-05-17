#ifndef SPHERESANIMATOR_H_
#define SPHERESANIMATOR_H_

#include <Behaviour.h>
#include <Texture.h>

#include <glm/glm.hpp>

COMPONENT(SpheresAnimator, Behaviour)
public:
	virtual void OnInstantiate()
	{
		mRotationSpeed = 0.0f;
		mRotationAxis = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	inline void SetSpheres(std::vector<GameObject*> spheres)
	{
		mSpheres = spheres;
	}

	inline void SetRotationSpeed(float rotationSpeed)
	{
		mRotationSpeed = rotationSpeed;
	}

	inline void SetRotationAxis(const glm::vec3& rotationAxis)
	{
		mRotationAxis = rotationAxis;
	}

	virtual void OnUpdate(float time, float deltaTime);

private:
	std::vector<GameObject*> mSpheres;
	float mRotationSpeed;
	glm::vec3 mRotationAxis;

};

#endif