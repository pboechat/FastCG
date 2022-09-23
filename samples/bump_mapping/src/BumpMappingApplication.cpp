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
#include <FastCG/FlyCameraController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Colors.h>

BumpMappingApplication::BumpMappingApplication() :
	Application({ "bump_mapping", 1024, 768, 60, RenderingPath::RP_FORWARD_RENDERING })
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
		rMaterials.emplace_back(pMeshFilter->GetMaterial());
		GetMaterialsRecursively(pChildGameObject, rMaterials);
	}
}

void CreateSceneLights(std::vector<GameObject*>& rSceneLights)
{
	auto* pSceneLights = GameObject::Instantiate();

	auto* pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(2, 2, 0));
	rSceneLights.emplace_back(pLightGameObject);

	auto* pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(Colors::WHITE);
	pPointLight->SetSpecularColor(Colors::WHITE);
	pPointLight->SetIntensity(1);
	pPointLight->SetQuadraticAttenuation(0.25f);

	pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(-2, 2, 0));
	rSceneLights.emplace_back(pLightGameObject);

	pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(Colors::RED);
	pPointLight->SetSpecularColor(Colors::RED);
	pPointLight->SetIntensity(1);
	pPointLight->SetQuadraticAttenuation(0.25f);
}

void LoadModel(std::vector<std::shared_ptr<Material>>& rModelMaterials)
{
	auto* pModel = ModelImporter::Import("objs/Doomsday.obj");
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
	auto center = bounds.getCenter();
	pTransform->SetPosition(glm::vec3(-center.x * scale, 0, -center.z * scale));
}

void BumpMappingApplication::OnStart()
{
	GetMainCamera()->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, 0.5f, 1));

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

	auto* pFlyCameraController = FlyCameraController::Instantiate(pPlayerGameObject);
	pFlyCameraController->SetWalkSpeed(5);
	pFlyCameraController->SetTurnSpeed(5);
}

void BumpMappingApplication::CreateGround()
{
	auto* pGround = GameObject::Instantiate();

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
	mpGroundMaterial->SetFloat("shininess", 30);

	auto* pMeshFilter = MeshFilter::Instantiate(pGround);
	pMeshFilter->SetMaterial(mpGroundMaterial);
}
