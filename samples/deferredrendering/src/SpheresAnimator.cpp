#include "SpheresAnimator.h"

#include <Transform.h>

COMPONENT_IMPLEMENTATION(SpheresAnimator, Behaviour);

void SpheresAnimator::OnUpdate(float time, float deltaTime)
{
	for (unsigned int i = 0; i < mSpheres.size(); i++)
	{
		GameObject* pSphere = mSpheres[i];
		pSphere->GetTransform()->RotateAround(mRotationSpeed * deltaTime, mRotationAxis);
	}
}
