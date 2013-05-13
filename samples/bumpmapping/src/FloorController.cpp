#include "FloorController.h"

#include <Input.h>
#include <MouseButton.h>
#include <Material.h>
#include <MeshFilter.h>
#include <ShaderRegistry.h>

COMPONENT_IMPLEMENTATION(FloorController, Behaviour);

void FloorController::OnUpdate(float time, float deltaTime)
{
	if (Input::GetMouseButton(MouseButton::LEFT_BUTTON) && time - mLastLeftButtonClickTime > 0.5f)
	{
		mCurrentTextureIndex = (++mCurrentTextureIndex % mColorMapTextures.size());
		mLastLeftButtonClickTime = time;

		MeshFilter* pMeshFilter = dynamic_cast<MeshFilter*>(GetGameObject()->GetComponent(MeshFilter::TYPE));

#ifdef USE_PROGRAMMABLE_PIPELINE
		pMeshFilter->GetMaterial()->SetTexture("colorMap", mColorMapTextures[mCurrentTextureIndex]);
		pMeshFilter->GetMaterial()->SetTexture("bumpMap", mBumpMapTextures[mCurrentTextureIndex]);
#else
		pMeshFilter->GetMaterial()->SetTexture(mColorMapTextures[mCurrentTextureIndex]);
#endif
	}

#ifdef USE_PROGRAMMABLE_PIPELINE

	else if (Input::GetMouseButton(MouseButton::MIDDLE_BUTTON) && time - mLastMiddleButtonClickTime > 0.5f)
	{
		
		MeshFilter* pMeshFilter = dynamic_cast<MeshFilter*>(GetGameObject()->GetComponent(MeshFilter::TYPE));

		Shader* pFloorShader;
		if (pMeshFilter->GetMaterial()->GetShader()->GetName() == "BumpedSpecular")
		{
			pFloorShader = ShaderRegistry::Find("Specular");
		}

		else
		{
			pFloorShader = ShaderRegistry::Find("BumpedSpecular");
		}

		pMeshFilter->GetMaterial()->SetShader(pFloorShader);

		mLastMiddleButtonClickTime = time;
	}

	else if (Input::GetMouseWheelDelta() != 0)
	{
		float mouseWheelDelta = Input::GetMouseWheelDelta() * deltaTime;
		mTiling += glm::vec2(mouseWheelDelta, mouseWheelDelta);
		MeshFilter* pMeshFilter = dynamic_cast<MeshFilter*>(GetGameObject()->GetComponent(MeshFilter::TYPE));
		Material* pFloorMaterial = pMeshFilter->GetMaterial();
		pFloorMaterial->SetTextureTiling("colorMap", mTiling);
		pFloorMaterial->SetTextureTiling("bumpMap", mTiling);
	}

#endif
}
