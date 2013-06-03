#include "SSAOApplication.h"
#include "KeyBindings.h"

#include <DirectionalLight.h>
//#include <PointLight.h>
#include <ShaderRegistry.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <PointLight.h>
#include <FirstPersonCameraController.h>
#include <ModelImporter.h>
#include <StandardGeometries.h>
#include <Colors.h>
#include <MathT.h>

SSAOApplication::SSAOApplication() :
	Application("ssao", 1024, 768, 30, true),
	mpGroundMesh(0),
	mpGroundMaterial(0)
{
	mGlobalAmbientLight = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
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
}

void SSAOApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.0f, 3.0f, 2.0f));
	mpMainCamera->GetGameObject()->GetTransform()->Rotate(glm::vec3(-45.0f, 0.0f, 0.0f));
	mpMainCamera->SetSSAOEnabled(true);

	GameObject* pSceneLights = GameObject::Instantiate();

	GameObject* pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(0.0f, -1.0f, -1.0f));

	DirectionalLight* pDirectionalLight = DirectionalLight::Instantiate(pLightGameObject);
	pDirectionalLight->SetIntensity(1.0f);

	/*GameObject* pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(0.0f, 1.0f, 1.0f));

	PointLight* pPointLight = PointLight::Instantiate(pLightGameObject);
	pPointLight->SetIntensity(1.0f);*/

	ModelImporter::LogToConsole();

	GameObject* pModel = ModelImporter::Import("models/Bane.obj");

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

	mpGroundMaterial = new Material(ShaderRegistry::Find("SolidColor"));
	mpGroundMaterial->SetVec4("diffuseColor", Colors::WHITE);

	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pGround);
	pMeshFilter->SetMaterial(mpGroundMaterial);

	GameObject* pGameObject = GameObject::Instantiate();

	KeyBindings* pKeyBindings = KeyBindings::Instantiate(pGameObject);
	pKeyBindings->SetSceneLights(pSceneLights);
	pKeyBindings->SetSceneModel(pGround);
	pKeyBindings->SetLightDistance(1.0f);

	FirstPersonCameraController* pController = FirstPersonCameraController::Instantiate(pGameObject);
	pController->SetWalkSpeed(10.0f);
	pController->SetTurnSpeed(100.0f);
	pController->SetFlying(true);
}
