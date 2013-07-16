#include "SpheresController.h"

#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <ShaderRegistry.h>
#include <MeshFilter.h>

COMPONENT_IMPLEMENTATION(SpheresController, Behaviour);

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

			MeshFilter* pMeshFilter = dynamic_cast<MeshFilter*>(pSphere->GetComponent(MeshFilter::TYPE));

			pMeshFilter->GetMaterial()->SetTexture("colorMap", mColorMapTextures[mCurrentTextureIndex]);
			pMeshFilter->GetMaterial()->SetTexture("bumpMap", mBumpMapTextures[mCurrentTextureIndex]);
		}

		mLastLeftButtonClickTime = time;
	}
	else if (Input::GetMouseButton(MouseButton::MIDDLE_BUTTON) && time - mLastMiddleButtonClickTime > 0.5f)
	{
		GameObject* pSphere = mSpheres[0];
		MeshFilter* pMeshFilter = dynamic_cast<MeshFilter*>(pSphere->GetComponent(MeshFilter::TYPE));

		Shader* pSphereShader;
		if (pMeshFilter->GetMaterial()->GetShader()->GetName() == "BumpedSpecular")
		{
			pSphereShader = ShaderRegistry::Find("Specular");
		}

		else
		{
			pSphereShader = ShaderRegistry::Find("BumpedSpecular");
		}

		pMeshFilter->GetMaterial()->SetShader(pSphereShader);

		mLastMiddleButtonClickTime = time;
	}

	for (unsigned int i = 0; i < mSpheres.size(); i++)
	{
		GameObject* pSphere = mSpheres[i];
		pSphere->GetTransform()->RotateAround(mRotationSpeed * deltaTime, mRotationAxis);
	}
}
