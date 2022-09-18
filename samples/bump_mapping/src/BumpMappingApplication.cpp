#include "BumpMappingApplication.h"
#include "Controls.h"

#include <FastCG/TextureImporter.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/ShaderRegistry.h>
#include <FastCG/PointLight.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MeshRenderer.h>
#include <FastCG/MeshFilter.h>
#include <FastCG/MathT.h>
#include <FastCG/FirstPersonCameraController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Colors.h>

BumpMappingApplication::BumpMappingApplication() :
	Application({ "bump_mapping" })
{
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void GetMaterialsRecursively(const GameObject* pGameObject, std::vector<std::shared_ptr<Material>>& rMaterials)
{
	for (auto* pChild : pGameObject->GetTransform()->GetChildren())
	{
		const auto* pChildGameObject = pChild->GetGameObject();
		auto* pMeshFilter = static_cast<MeshFilter*>(pChildGameObject->GetComponent(MeshFilter::TYPE));
		if (pMeshFilter == nullptr)
		{
			continue;
		}
		rMaterials.push_back(pMeshFilter->GetMaterial());
		GetMaterialsRecursively(pChildGameObject, rMaterials);
	}
}

void CreateSceneLights(std::vector<GameObject*>& rSceneLights)
{
	auto* pSceneLights = GameObject::Instantiate();

	auto* pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(2, 2, 0));
	rSceneLights.push_back(pLightGameObject);

	auto* pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(Colors::WHITE);
	pPointLight->SetSpecularColor(Colors::WHITE);
	pPointLight->SetQuadraticAttenuation(0.25f);

	pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(-2, 2, 0));
	rSceneLights.push_back(pLightGameObject);

	pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(Colors::RED);
	pPointLight->SetSpecularColor(Colors::RED);
	pPointLight->SetQuadraticAttenuation(0.25f);
}

void LoadModel(std::vector<std::shared_ptr<Material>>& rModelMaterials)
{
	auto pModel = ModelImporter::Import("objs/Doomsday.obj");

	if (pModel == nullptr)
	{
		THROW_EXCEPTION(Exception, "Missing doomsday model");
	}

	GetMaterialsRecursively(pModel, rModelMaterials);

	const auto& bounds = pModel->GetBounds();
	auto* pTransform = pModel->GetTransform();
	float scale = 0;
	scale = bounds.max.x - bounds.min.x;
	scale = MathF::Max(bounds.max.y - bounds.min.y, scale);
	scale = MathF::Max(bounds.max.z - bounds.min.z, scale);
	scale = 1.0f / scale;

	pTransform->SetScale(glm::vec3(scale, scale, scale));
	pTransform->SetPosition(-bounds.getCenter() * scale);
}

void BumpMappingApplication::OnStart()
{
	GetMainCamera()->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, 0, 1));

	std::vector<GameObject*> sceneLights;
	CreateSceneLights(sceneLights);

	std::vector<std::shared_ptr<Material>> modelMaterials;
	LoadModel(modelMaterials);

	CreateGround();

	auto* pGameObject = GameObject::Instantiate();

	auto* pControls = Controls::Instantiate(pGameObject);
	pControls->SetSceneLights(sceneLights);
	pControls->SetModelMaterials(modelMaterials);

	auto* pPlayerGameObject = GameObject::Instantiate();

	auto* pFirstPersonCameraController = FirstPersonCameraController::Instantiate(pPlayerGameObject);
	pFirstPersonCameraController->SetWalkSpeed(1);
	pFirstPersonCameraController->SetTurnSpeed(90);
	pFirstPersonCameraController->SetFlying(true);
}

void BumpMappingApplication::CreateGround()
{
	auto* pGround = GameObject::Instantiate();
	pGround->GetTransform()->SetPosition(glm::vec3(0, -0.5f, 0));

	mpGroundMesh = StandardGeometries::CreateXZPlane(5, 5);
	auto* pMeshRenderer = MeshRenderer::Instantiate(pGround);
	pMeshRenderer->AddMesh(mpGroundMesh);

	mpGroundColorMapTexture = TextureImporter::Import("textures/GroundColorMap.png");
	mpGroundBumpMapTexture = TextureImporter::Import("textures/GroundBumpMap.png");

	mpGroundMaterial = std::make_shared<Material>(ShaderRegistry::Find("BumpedSpecular"));
	mpGroundMaterial->SetTexture("colorMap", mpGroundColorMapTexture);
	mpGroundMaterial->SetTextureTiling("colorMap", glm::vec2(4, 4));
	mpGroundMaterial->SetTexture("bumpMap", mpGroundBumpMapTexture);
	mpGroundMaterial->SetTextureTiling("bumpMap", glm::vec2(4, 4));
	mpGroundMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpGroundMaterial->SetVec4("specularColor", Colors::WHITE);
	mpGroundMaterial->SetFloat("shininess", 5);

	auto* pMeshFilter = MeshFilter::Instantiate(pGround);
	pMeshFilter->SetMaterial(mpGroundMaterial);
}
