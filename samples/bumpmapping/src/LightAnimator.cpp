#include "LightAnimator.h"

#include <Transform.h>

IMPLEMENT_TYPE(LightAnimator, Behaviour);

void LightAnimator::OnUpdate(float time, float deltaTime)
{
	glm::vec3 position = GetGameObject()->GetTransform()->GetPosition() + (mDirection * mSpeed * deltaTime);

	if (position.z <= -mHalfAmplitude || position.z >= mHalfAmplitude)
	{
		mDirection *= -1.0f;
	}

	GetGameObject()->GetTransform()->SetPosition(position);
}
