#include "FloorController.h"

#include <Input.h>
#include <MouseButton.h>
#include <Material.h>
#include <MeshFilter.h>
#include <ShaderRegistry.h>

IMPLEMENT_TYPE(FloorController, Behaviour);

void FloorController::OnUpdate(float time, float deltaTime)
{
	if (Input::GetMouseButton(MouseButton::LEFT_BUTTON) && time - mLastLeftButtonClickTime > 0.5f)
	{
		mCurrentTextureIndex = (++mCurrentTextureIndex % mColorMapTextures.size());
		mLastLeftButtonClickTime = time;

		MeshFilterPtr meshFilterPtr = DynamicCast<MeshFilter>(GetGameObject()->GetComponent(MeshFilter::TYPE));

#ifdef USE_PROGRAMMABLE_PIPELINE
		meshFilterPtr->GetMaterial()->SetTexture("colorMap", mColorMapTextures[mCurrentTextureIndex]);
		meshFilterPtr->GetMaterial()->SetTexture("bumpMap", mBumpMapTextures[mCurrentTextureIndex]);
#else
		meshFilterPtr->GetMaterial()->SetTexture(mColorMapTextures[mCurrentTextureIndex]);
#endif
	}

#ifdef USE_PROGRAMMABLE_PIPELINE

	else if (Input::GetMouseButton(MouseButton::MIDDLE_BUTTON) && time - mLastMiddleButtonClickTime > 0.5f)
	{
		
		MeshFilterPtr meshFilterPtr = DynamicCast<MeshFilter>(GetGameObject()->GetComponent(MeshFilter::TYPE));

		ShaderPtr floorShaderPtr;
		if (meshFilterPtr->GetMaterial()->GetShader()->GetName() == "BumpedSpecular")
		{
			floorShaderPtr = ShaderRegistry::Find("Specular");
		}

		else
		{
			floorShaderPtr = ShaderRegistry::Find("BumpedSpecular");
		}

		meshFilterPtr->GetMaterial()->SetShader(floorShaderPtr);

		mLastMiddleButtonClickTime = time;
	}

	else if (Input::GetMouseWheelDelta() != 0)
	{
		float mouseWheelDelta = Input::GetMouseWheelDelta() * deltaTime;
		mTiling += glm::vec2(mouseWheelDelta, mouseWheelDelta);
		MeshFilterPtr meshFilterPtr = DynamicCast<MeshFilter>(GetGameObject()->GetComponent(MeshFilter::TYPE));
		MaterialPtr floorMaterialPtr = meshFilterPtr->GetMaterial();
		floorMaterialPtr->SetTextureTiling("colorMap", mTiling);
		floorMaterialPtr->SetTextureTiling("bumpMap", mTiling);
	}

#endif
}
