#include "SpheresController.h"

#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <ShaderRegistry.h>

SpheresController::SpheresController(const std::vector<TriangleMeshPtr>& rSpheresPtrs, float rotationSpeed, const glm::vec3& rRotationAxis, const std::vector<TexturePtr>& rColorMapTexturesPtrs, const std::vector<TexturePtr>& rBumpMapTexturesPtrs) :
	mSpheresPtrs(rSpheresPtrs),
	mRotationSpeed(rotationSpeed),
	mRotationAxis(rRotationAxis),
	mColorMapTexturePtrs(rColorMapTexturesPtrs),
	mBumpMapTexturePtrs(rBumpMapTexturesPtrs),
	mLastLeftButtonClickTime(0.0f),
	mLastF1PressTime(0.0f),
	mLastMiddleButtonClickTime(0.0f),
	mCurrentTextureIndex(0)
{
}

void SpheresController::Update(float time, float deltaTime)
{
	if (Input::GetMouseButton(MouseButton::LEFT_BUTTON) && time - mLastLeftButtonClickTime > 0.333f)
	{
		mCurrentTextureIndex = (++mCurrentTextureIndex % mColorMapTexturePtrs.size());
		mLastLeftButtonClickTime = time;
	}

	if (Input::GetKey(KeyCode::F1) && time - mLastF1PressTime > 0.333f)
	{
		for (unsigned int i = 0; i < mSpheresPtrs.size(); i++)
		{
			TriangleMeshPtr spherePtr = mSpheresPtrs[i];
			spherePtr->SetEnabled(!spherePtr->IsEnabled());
		}
		mLastF1PressTime = time;
	}

#ifdef USE_PROGRAMMABLE_PIPELINE

	else if (Input::GetMouseButton(MouseButton::MIDDLE_BUTTON) && time - mLastMiddleButtonClickTime > 0.333f)
	{
		MaterialPtr sphereMaterialPtr = mSpheresPtrs[0]->GetMaterial();
		ShaderPtr sphereShaderPtr;

		if (sphereMaterialPtr->GetShader()->GetName() == "BumpedSpecular")
		{
			sphereShaderPtr = ShaderRegistry::Find("Specular");
		}

		else
		{
			sphereShaderPtr = ShaderRegistry::Find("BumpedSpecular");
		}

		for (unsigned int i = 0; i < mSpheresPtrs.size(); i++)
		{
			TriangleMeshPtr spherePtr = mSpheresPtrs[i];
			spherePtr->GetMaterial()->SetShader(sphereShaderPtr);
		}

		mLastMiddleButtonClickTime = time;
	}

#endif

	for (unsigned int i = 0; i < mSpheresPtrs.size(); i++)
	{
		TriangleMeshPtr spherePtr = mSpheresPtrs[i];
#ifdef USE_PROGRAMMABLE_PIPELINE
		spherePtr->GetMaterial()->SetTexture("colorMap", mColorMapTexturePtrs[mCurrentTextureIndex]);
		spherePtr->GetMaterial()->SetTexture("bumpMap", mBumpMapTexturePtrs[mCurrentTextureIndex]);
#else
		spherePtr->GetMaterial()->SetTexture(mColorMapTexturePtrs[mCurrentTextureIndex]);
#endif
		spherePtr->Rotate(mRotationSpeed * deltaTime, mRotationAxis);
	}
}
