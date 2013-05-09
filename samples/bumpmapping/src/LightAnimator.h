#ifndef LIGHTANIMATOR_H_
#define LIGHTANIMATOR_H_

#include <Updateable.h>
#include <Light.h>

#include <glm/glm.hpp>

class LightAnimator : public Updateable
{
public:
	LightAnimator(LightPtr lightPtr, float speed, float amplitude, glm::vec3 direction);

	virtual void Update(float time, float deltaTime);

private:
	LightPtr mLightPtr;
	float mSpeed;
	float mHalfAmplitude;
	glm::vec3 mDirection;

};

#endif