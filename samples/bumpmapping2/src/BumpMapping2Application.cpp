#include "BumpMapping2Application.h"
#include "KeyBindings.h"

#include <DirectionalLight.h>
#include <PointLight.h>
#include <ShaderRegistry.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <PointLight.h>
#include <FirstPersonCameraController.h>
#include <ModelImporter.h>
#include <TextureImporter.h>
#include <StandardGeometries.h>
#include <Colors.h>
#include <MathT.h>

BumpMapping2Application::BumpMapping2Application() :
	Application("bumpmapping2", 1024, 768, 60, false, "../../core/"),
	mpGroundMesh(0),
	mpGroundMaterial(0),
	mpGroundColorMapTexture(0),
	mpGroundBumpMapTexture(0)
{
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

BumpMapping2Application::~BumpMapping2Application()
{
	if (mpGroundMesh != 0)
	{
		delete mpGroundMesh;
	}

	if (mpGroundMaterial != 0)
	{
		delete mpGroundMaterial;
	}

	if (mpGroundColorMapTexture != 0)
	{
		delete mpGroundColorMapTexture;
	}

	if (mpGroundBumpMapTexture != 0)
	{
		delete mpGroundBumpMapTexture;
	}
}

void BumpMapping2Application::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, 0, 1.0f));

	std::vector<GameObject*> sceneLights;
	CreateSceneLights(sceneLights);

	std::vector<Material*> modelMaterials;
	LoadModel(modelMaterials);

	CreateGround();

	GameObject* pGameObject = GameObject::Instantiate();

	KeyBindings* pKeyBindings = KeyBindings::Instantiate(pGameObject);
	pKeyBindings->SetSceneLights(sceneLights);
	pKeyBindings->SetModelMaterials(modelMaterials);

	GameObject* pPlayerGameObject = GameObject::Instantiate();

	FirstPersonCameraController* pFirstPersonCameraController = FirstPersonCameraController::Instantiate(pPlayerGameObject);
	pFirstPersonCameraController->SetWalkSpeed(5.0f);
	pFirstPersonCameraController->SetTurnSpeed(60.0f);
	pFirstPersonCameraController->SetFlying(true);
}

void BumpMapping2Application::GetMaterialsRecursively(const GameObject* pGameObject, std::vector<Material*>& rMaterials) const
{
	const std::vector<Transform*>& rChildren = pGameObject->GetTransform()->GetChildren();

	for (unsigned int i = 0; i < rChildren.size(); i++)
	{
		const GameObject* pChildGameObject = rChildren[i]->GetGameObject();
		MeshFilter* pMeshFilter = dynamic_cast<MeshFilter*>(pChildGameObject->GetComponent(MeshFilter::TYPE));

		if (pMeshFilter == 0)
		{
			continue;
		}

		rMaterials.push_back(pMeshFilter->GetMaterial());

		GetMaterialsRecursively(pChildGameObject, rMaterials);
	}
}

void BumpMapping2Application::CreateSceneLights(std::vector<GameObject*>& rSceneLights) const
{
	GameObject* pSceneLights = GameObject::Instantiate();

	GameObject* pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(2.0f, 2.0f, 0.0f));
	rSceneLights.push_back(pLightGameObject);

	PointLight* pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(Colors::WHITE);
	pPointLight->SetSpecularColor(Colors::WHITE);
	pPointLight->SetQuadraticAttenuation(0.25f);

	pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(-2.0f, 2.0f, 0.0f));
	rSceneLights.push_back(pLightGameObject);

	pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(Colors::RED);
	pPointLight->SetSpecularColor(Colors::RED);
	pPointLight->SetQuadraticAttenuation(0.25f);
}

void BumpMapping2Application::LoadModel(std::vector<Material*>& rModelMaterials) const
{
	ModelImporter::LogToConsole();

	GameObject* pModel = ModelImporter::Import("models/Doomsday.obj");

	GetMaterialsRecursively(pModel, rModelMaterials);

	const AABB& boundingVolume = pModel->GetBoundingVolume();
	Transform* pTransform = pModel->GetTransform();
	float scale = 0;
	scale = boundingVolume.max.x - boundingVolume.min.x;
	scale = MathF::Max(boundingVolume.max.y - boundingVolume.min.y, scale);
	scale = MathF::Max(boundingVolume.max.z - boundingVolume.min.z, scale);
	scale = 1.0f / scale;

	pTransform->SetScale(glm::vec3(scale, scale, scale));
	pTransform->SetPosition(-boundingVolume.center * scale);
}

void BumpMapping2Application::CreateGround()
{
	GameObject* pGround = GameObject::Instantiate();
	pGround->GetTransform()->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));

	mpGroundMesh = StandardGeometries::CreateXZPlane(5.0f, 5.0f);
	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pGround);
	pMeshRenderer->AddMesh(mpGroundMesh);

	mpGroundColorMapTexture = TextureImporter::Import("textures/GroundColorMap.png");
	mpGroundBumpMapTexture = TextureImporter::Import("textures/GroundBumpMap.png");

	mpGroundMaterial = new Material(ShaderRegistry::Find("BumpedSpecular"));
	mpGroundMaterial->SetTexture("colorMap", mpGroundColorMapTexture);
	mpGroundMaterial->SetTextureTiling("colorMap", glm::vec2(4.0f, 4.0f));
	mpGroundMaterial->SetTexture("bumpMap", mpGroundBumpMapTexture);
	mpGroundMaterial->SetTextureTiling("bumpMap", glm::vec2(4.0f, 4.0f));
	mpGroundMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpGroundMaterial->SetVec4("specularColor", Colors::WHITE);
	mpGroundMaterial->SetFloat("shininess", 5.0f);

	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pGround);
	pMeshFilter->SetMaterial(mpGroundMaterial);
}
