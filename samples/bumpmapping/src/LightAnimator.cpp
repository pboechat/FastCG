#include "LightAnimator.h"

#include <Transform.h>

COMPONENT_IMPLEMENTATION(LightAnimator, Behaviour);

void LightAnimator::OnUpdate(float time, float deltaTime)
{
	glm::vec3 move = mDirection * mSpeed * deltaTime;
	glm::vec3 newPosition = GetGameObject()->GetTransform()->GetWorldPosition() + move;

	float length = glm::length(newPosition);
	if (length >= mHalfAmplitude)
	{
		mDirection *= -1.0f;
	} 
	else
	{
		GetGameObject()->GetTransform()->SetWorldPosition(newPosition);
	}
}
