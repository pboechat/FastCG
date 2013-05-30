#include "ModelImportApplication.h"

#include <DirectionalLight.h>
#include <ModelImporter.h>
#include <FirstPersonCameraController.h>
#include <MathT.h>

ModelImportApplication::ModelImportApplication() :
	Application("modelimport", 1024, 768, 30)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void ModelImportApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));

	GameObject* pGameObject = GameObject::Instantiate();

	DirectionalLight* pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pGameObject->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
	pDirectionalLight->SetAmbientColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectionalLight->SetDiffuseColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectionalLight->SetSpecularColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectionalLight->SetIntensity(1.0f);

	pGameObject = ModelImporter::Import("models/OogieBoogie.obj");

	const AABB& boundingVolume = pGameObject->GetBoundingVolume();

	Transform* pTransform = pGameObject->GetTransform();
	float scale = 0;
	scale = boundingVolume.max.x - boundingVolume.min.x;
	scale = MathF::Max(boundingVolume.max.y - boundingVolume.min.y, scale);
	scale = MathF::Max(boundingVolume.max.z - boundingVolume.min.z, scale);
	scale = 1.0f / scale;

	pTransform->ScaleLocal(glm::vec3(scale, scale, scale));
	pTransform->SetPosition(-boundingVolume.center * scale);

	pGameObject = GameObject::Instantiate();

	FirstPersonCameraController* pFirstPersonCameraController = FirstPersonCameraController::Instantiate(pGameObject);
	pFirstPersonCameraController->SetWalkSpeed(10.0f);
	pFirstPersonCameraController->SetTurnSpeed(100.0f);
	pFirstPersonCameraController->SetFlying(true);
}
