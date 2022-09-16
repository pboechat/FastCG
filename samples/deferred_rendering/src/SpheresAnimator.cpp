#include "SpheresAnimator.h"

#include <FastCG/Transform.h>

IMPLEMENT_COMPONENT(SpheresAnimator, Behaviour);

void SpheresAnimator::OnUpdate(float time, float deltaTime)
{
	for (auto* pSphere : mSpheres)
	{
		pSphere->GetTransform()->RotateAround(mRotationSpeed * deltaTime, mRotationAxis);
	}
}
