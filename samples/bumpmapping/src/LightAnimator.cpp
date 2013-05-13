#include "LightAnimator.h"

#include <Transform.h>

COMPONENT_IMPLEMENTATION(LightAnimator, Behaviour);

void LightAnimator::OnUpdate(float time, float deltaTime)
{
	glm::vec3 position = GetGameObject()->GetTransform()->GetPosition() + (mDirection * mSpeed * deltaTime);

	if (position.z <= -mHalfAmplitude)
	{
		position.z = -mHalfAmplitude;
		mDirection.z = 1.0f;
	} 
	else if (position.z >= mHalfAmplitude)
	{
		position.z = mHalfAmplitude;
		mDirection.z = -1.0f;
	}

	GetGameObject()->GetTransform()->SetPosition(position);
}
