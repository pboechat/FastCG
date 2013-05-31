#include "SSAOApplication.h"
#include "KeyBindings.h"

//#include <DirectionalLight.h>
#include <PointLight.h>
#include <FirstPersonCameraController.h>
#include <ModelImporter.h>
#include <Colors.h>
#include <MathT.h>

SSAOApplication::SSAOApplication() :
	Application("ssao", 1024, 768, 30, true)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void SSAOApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));
	mpMainCamera->SetSSAOEnabled(true);

	GameObject* pGameObject;

	pGameObject = GameObject::Instantiate();

	DirectionalLight* pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pGameObject->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
	pDirectionalLight->SetAmbientColor(Colors::WHITE);
	pDirectionalLight->SetDiffuseColor(Colors::WHITE);
	pDirectionalLight->SetSpecularColor(Colors::WHITE);
	pDirectionalLight->SetIntensity(1.0f);

	ModelImporter::LogToConsole();

	pGameObject = ModelImporter::Import("models/Doomsday.obj");

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

	KeyBindings* pKeyBindings = KeyBindings::Instantiate(pGameObject);

	FirstPersonCameraController* pController = FirstPersonCameraController::Instantiate(pGameObject);
	pController->SetWalkSpeed(10.0f);
	pController->SetTurnSpeed(100.0f);
	pController->SetFlying(true);
}
