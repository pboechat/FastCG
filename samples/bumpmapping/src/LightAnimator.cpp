#include "LightAnimator.h"

LightAnimator::LightAnimator(LightPtr lightPtr, float speed, float amplitude, glm::vec3 direction) :
	mLightPtr(lightPtr),
	mSpeed(speed),
	mHalfAmplitude(amplitude * 0.5f),
	mDirection(direction)
{
}

void LightAnimator::Update(float time, float deltaTime)
{
	glm::vec3 position = mLightPtr->GetPosition() + mDirection * mSpeed * deltaTime;

	if (position.z <= -mHalfAmplitude || position.z >= mHalfAmplitude)
	{
		mDirection *= -1.0f;
	}

	mLightPtr->SetPosition(position);
}
