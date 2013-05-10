#include "SpheresController.h"

#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <ShaderRegistry.h>
#include <MeshFilter.h>

void SpheresController::OnUpdate(float time, float deltaTime)
{
	if (Input::GetKey(KeyCode::F1) && time - mLastKeyPressTime > 0.5f)
	{
		for (unsigned int i = 0; i < mSpheres.size(); i++)
		{
			GameObject* pSphere = mSpheres[i];
			pSphere->SetActive(!pSphere->IsActive());
		}

		mLastKeyPressTime = time;
	}

	if (Input::GetMouseButton(MouseButton::LEFT_BUTTON) && time - mLastLeftButtonClickTime > 0.5f)
	{
		mCurrentTextureIndex = (++mCurrentTextureIndex % mColorMapTextures.size());

		for (unsigned int i = 0; i < mSpheres.size(); i++)
		{
			GameObject* pSphere = mSpheres[i];

			MeshFilterPtr meshFilterPtr = DynamicCast<MeshFilter>(pSphere->GetComponent(MeshFilter::TYPE));

#ifdef USE_PROGRAMMABLE_PIPELINE
			meshFilterPtr->GetMaterial()->SetTexture("colorMap", mColorMapTextures[mCurrentTextureIndex]);
			meshFilterPtr->GetMaterial()->SetTexture("bumpMap", mBumpMapTextures[mCurrentTextureIndex]);
#else
			meshFilterPtr->GetMaterial()->SetTexture(mColorMapTextures[mCurrentTextureIndex]);
#endif
		}

		mLastLeftButtonClickTime = time;
	}
#ifdef USE_PROGRAMMABLE_PIPELINE
	else if (Input::GetMouseButton(MouseButton::MIDDLE_BUTTON) && time - mLastMiddleButtonClickTime > 0.5f)
	{
		GameObjectPtr spherePtr = mSpheres[0];
		MeshFilterPtr meshFilter = DynamicCast<MeshFilter>(spherePtr->GetComponent(MeshFilter::TYPE));

		ShaderPtr sphereShaderPtr;
		if (meshFilter->GetMaterial()->GetShader()->GetName() == "BumpedSpecular")
		{
			sphereShaderPtr = ShaderRegistry::Find("Specular");
		}

		else
		{
			sphereShaderPtr = ShaderRegistry::Find("BumpedSpecular");
		}

		meshFilter->GetMaterial()->SetShader(sphereShaderPtr);

		mLastMiddleButtonClickTime = time;
	}
#endif

	for (unsigned int i = 0; i < mSpheres.size(); i++)
	{
		GameObject* pSphere = mSpheres[i];
		pSphere->GetTransform()->Rotate(mRotationSpeed * deltaTime, mRotationAxis);
	}
}
