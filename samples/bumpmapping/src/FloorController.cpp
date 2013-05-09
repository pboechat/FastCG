#include "FloorController.h"

#include <Input.h>
#include <MouseButton.h>
#include <ShaderRegistry.h>

FloorController::FloorController(TriangleMeshPtr floorPtr, const std::vector<TexturePtr>& rColorMapTexturesPtrs, const std::vector<TexturePtr>& rBumpMapTexturesPtrs) :
	mFloorPtr(floorPtr),
	mColorMapTexturesPtrs(rColorMapTexturesPtrs),
	mBumpMapTexturesPtrs(rBumpMapTexturesPtrs),
	mCurrentTextureIndex(0),
	mTiling(1, 1),
	mLastLeftButtonClickTime(0.0f),
	mLastMiddleButtonClickTime(0.0f)
{
	mFloorMaterialPtr = mFloorPtr->GetMaterial();
}

void FloorController::Update(float time, float deltaTime)
{
	if (Input::GetMouseButton(MouseButton::LEFT_BUTTON) && time - mLastLeftButtonClickTime > 0.333f)
	{
		mCurrentTextureIndex = (++mCurrentTextureIndex % mColorMapTexturesPtrs.size());
		mLastLeftButtonClickTime = time;
	}

#ifdef USE_PROGRAMMABLE_PIPELINE

	else if (Input::GetMouseButton(MouseButton::MIDDLE_BUTTON) && time - mLastMiddleButtonClickTime > 0.333f)
	{
		ShaderPtr floorShaderPtr;

		if (mFloorMaterialPtr->GetShader()->GetName() == "BumpedSpecular")
		{
			floorShaderPtr = ShaderRegistry::Find("Specular");
		}

		else
		{
			floorShaderPtr = ShaderRegistry::Find("BumpedSpecular");
		}

		mFloorMaterialPtr->SetShader(floorShaderPtr);

		mLastMiddleButtonClickTime = time;
	}

	else if (Input::GetMouseWheelDelta() != 0)
	{
		float mouseWheelDelta = Input::GetMouseWheelDelta() * deltaTime;
		mTiling += glm::vec2(mouseWheelDelta, mouseWheelDelta);
		mFloorMaterialPtr->SetTextureTiling("colorMap", mTiling);
		mFloorMaterialPtr->SetTextureTiling("bumpMap", mTiling);
	}

#endif
}
