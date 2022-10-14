#include "Controls.h"

#include <FastCG/ShaderRegistry.h>
#include <FastCG/Shader.h>
#include <FastCG/MathT.h>
#include <FastCG/KeyCode.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/Input.h>
#include <FastCG/Application.h>

IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
	if (Input::GetKey(KeyCode::ESCAPE))
	{
		Application::GetInstance()->Exit();
		return;
	}

	Application::GetInstance()->DrawText("Press 'F1' to toggle bump mapping", 10, 10, glm::vec4{0, 1, 0, 1});

	IsKeyPressed(KeyCode::F1, mPressedKeyMask, 0, [&]()
				 {
			for (auto& pMaterial : mMaterials)
			{
				const auto& pShader = pMaterial->GetShader();
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
			} });

	for (auto *pSceneLight : mSceneLights)
	{
		pSceneLight->GetTransform()->RotateAroundLocal(deltaTime * 100, glm::vec3(0, 1, 0));
	}
}
