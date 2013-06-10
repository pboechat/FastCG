#include "SSAOApplication.h"
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

#include <vector>

SSAOApplication::SSAOApplication() :
	Application("ssao", 1024, 768, 30, true),
	mpGroundMesh(0),
	mpGroundMaterial(0),
	mpGroundColorMapTexture(0),
	mpGroundBumpMapTexture(0)
{
	mGlobalAmbientLight = Colors::BLACK;
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

SSAOApplication::~SSAOApplication()
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

void SSAOApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.0f, 3.0f, 2.0f));
	mpMainCamera->GetGameObject()->GetTransform()->Rotate(glm::vec3(-45.0f, 0.0f, 0.0f));
	mpMainCamera->SetSSAOEnabled(true);

	GameObject* pSceneLights = GameObject::Instantiate();

	GameObject* pLightGameObject;

	pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(0.0f, -1.0f, -1.0f));

	DirectionalLight* pDirectionalLight = DirectionalLight::Instantiate(pLightGameObject);
	pDirectionalLight->SetIntensity(0.4f);

	std::vector<GameObject*> sceneLights;

	pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
	sceneLights.push_back(pLightGameObject);

	PointLight* pPointLight;

	pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
	pPointLight->SetSpecularColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
	pPointLight->SetIntensity(0.4f);

	pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(-1.0f, 1.0f, 1.0f));
	sceneLights.push_back(pLightGameObject);

	pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	pPointLight->SetSpecularColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	pPointLight->SetIntensity(0.4f);

	pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(-1.0f, 1.0f, -1.0f));
	sceneLights.push_back(pLightGameObject);

	pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	pPointLight->SetSpecularColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	pPointLight->SetIntensity(0.4f);

	pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(1.0f, 1.0f, -1.0f));
	sceneLights.push_back(pLightGameObject);

	pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetDiffuseColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	pPointLight->SetSpecularColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	pPointLight->SetIntensity(0.4f);

	ModelImporter::LogToConsole();

	GameObject* pModel = ModelImporter::Import("models/Joker.obj");

	const AABB& boundingVolume = pModel->GetBoundingVolume();
	Transform* pTransform = pModel->GetTransform();
	float scale = 0;
	scale = boundingVolume.max.x - boundingVolume.min.x;
	scale = MathF::Max(boundingVolume.max.y - boundingVolume.min.y, scale);
	scale = MathF::Max(boundingVolume.max.z - boundingVolume.min.z, scale);
	scale = 1.0f / scale;

	pTransform->ScaleLocal(glm::vec3(scale, scale, scale));
	pTransform->SetPosition(-boundingVolume.center * scale);

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
	mpGroundMaterial->SetVec4("specularColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	mpGroundMaterial->SetFloat("shininess", 5.0f);

	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pGround);
	pMeshFilter->SetMaterial(mpGroundMaterial);

	GameObject* pGameObject = GameObject::Instantiate();

	KeyBindings* pKeyBindings = KeyBindings::Instantiate(pGameObject);
	pKeyBindings->SetSceneLights(sceneLights);
	pKeyBindings->SetLightDistance(1.0f);

	FirstPersonCameraController* pController = FirstPersonCameraController::Instantiate(pGameObject);
	pController->SetWalkSpeed(10.0f);
	pController->SetTurnSpeed(100.0f);
	pController->SetFlying(true);
}
