#ifndef LIGHTANIMATOR_H_
#define LIGHTANIMATOR_H_

#include <Behaviour.h>

#include <glm/glm.hpp>

COMPONENT(LightAnimator, Behaviour)
public:
	virtual void OnInstantiate()
	{
		mSpeed = 0.0f;
		mHalfAmplitude = 0.0f;
		mDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	inline void SetSpeed(float speed)
	{
		mSpeed = speed;
	}

	inline void SetAmplitude(float amplitude)
	{
		mHalfAmplitude = amplitude * 0.5f;
	}

	inline void SetDirection(glm::vec3 direction)
	{
		mDirection = direction;
	}

	virtual void OnUpdate(float time, float deltaTime);

private:
	float mSpeed;
	float mHalfAmplitude;
	glm::vec3 mDirection;

};

#endif