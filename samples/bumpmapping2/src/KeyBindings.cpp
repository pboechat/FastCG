#include "KeyBindings.h"

#include <Application.h>
#include <FontRegistry.h>
#include <Input.h>
#include <KeyCode.h>
#include <Shader.h>
#include <ShaderRegistry.h>
#include <MathT.h>

#include <iostream>

COMPONENT_IMPLEMENTATION(KeyBindings, Behaviour);

void KeyBindings::OnUpdate(float time, float deltaTime)
{
#ifdef FIXED_FUNCTION_PIPELINE
	static unsigned int text1Height = 15;
	Application::GetInstance()->DrawText("Press 'F1' to toggle bump mapping", 12, 10, 15, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
#else
	Application::GetInstance()->DrawText("Press 'F1' to toggle bump mapping", FontRegistry::STANDARD_FONT_SIZE, 10, FontRegistry::STANDARD_FONT_SIZE, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
#endif
	
	if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
	}

	else if (Input::GetKey(KeyCode::F1) && time - mLastKeyPressTime > 0.333f)
	{
		for (unsigned int i = 0; i < mMaterials.size(); i++) 
		{
			Material* pMaterial = mMaterials[i];

			Shader* pShader = pMaterial->GetShader();
			if (pShader->GetName().find("Diffuse") != std::string::npos)
			{
				if (pShader->GetName() == "BumpedDiffuse")
				{
					pMaterial->SetShader(ShaderRegistry::Find("Diffuse"));
				}
				else 
				{
					pMaterial->SetShader(ShaderRegistry::Find("BumpedDiffuse"));
				}
			}
			else if (pShader->GetName().find("Specular") != std::string::npos)
			{
				if (pShader->GetName() == "BumpedSpecular")
				{
					pMaterial->SetShader(ShaderRegistry::Find("Specular"));
				}
				else 
				{
					pMaterial->SetShader(ShaderRegistry::Find("BumpedSpecular"));
				}
			}
		}
		mLastKeyPressTime = time;
	}

	for (unsigned int i = 0; i < mSceneLights.size(); i++)
	{
		mSceneLights[i]->GetTransform()->RotateAround(deltaTime * 100.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	}
}
