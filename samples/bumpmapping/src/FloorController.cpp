#include "FloorController.h"

#include <Application.h>
#include <Input.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <Material.h>
#include <MeshFilter.h>
#include <ShaderRegistry.h>
#include <FontRegistry.h>

COMPONENT_IMPLEMENTATION(FloorController, Behaviour);

void FloorController::OnUpdate(float time, float deltaTime)
{
	static unsigned int text1Height = FontRegistry::STANDARD_FONT_SIZE + 3;
	static unsigned int text2Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 2 + 3;
	static unsigned int text3Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 3 + 5;
	static unsigned int text4Height = (FontRegistry::STANDARD_FONT_SIZE + 3) * 4 + 7;
	Application::GetInstance()->DrawText("Press left mouse button to change textures", FontRegistry::STANDARD_FONT_SIZE, 10, text1Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Press middle mouse button to change to non-bumped shaders", FontRegistry::STANDARD_FONT_SIZE, 10, text2Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Use mouse wheel to change floor texture tiling", FontRegistry::STANDARD_FONT_SIZE, 10, text3Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Application::GetInstance()->DrawText("Use WASD/arrow keys and left mouse button to navigate", FontRegistry::STANDARD_FONT_SIZE, 10, text4Height, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	if (Input::GetMouseButton(MouseButton::LEFT_BUTTON) && time - mLastLeftButtonClickTime > 0.5f)
	{
		mCurrentTextureIndex = (++mCurrentTextureIndex % mColorMapTextures.size());
		mLastLeftButtonClickTime = time;

		MeshFilter* pMeshFilter = dynamic_cast<MeshFilter*>(GetGameObject()->GetComponent(MeshFilter::TYPE));

		pMeshFilter->GetMaterial()->SetTexture("colorMap", mColorMapTextures[mCurrentTextureIndex]);
		pMeshFilter->GetMaterial()->SetTexture("bumpMap", mBumpMapTextures[mCurrentTextureIndex]);
	}
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
	else if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
	}
}
