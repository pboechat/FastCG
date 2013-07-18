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
	Application("ssao", 1024, 768, 30, true, "../../core/")
{
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void SSAOApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, 2, 0));
	mpMainCamera->SetSSAOEnabled(true);

	GameObject* pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Rotate(glm::vec3(60, 90, 0));

	DirectionalLight* pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pDirectionalLight->SetDiffuseColor(Colors::WHITE);
	pDirectionalLight->SetSpecularColor(Colors::WHITE);
	pDirectionalLight->SetIntensity(1.0f);

	pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Rotate(glm::vec3(60, -90, 0));

	pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pDirectionalLight->SetDiffuseColor(Colors::WHITE);
	pDirectionalLight->SetSpecularColor(Colors::WHITE);
	pDirectionalLight->SetIntensity(1.0f);

	pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Rotate(glm::vec3(60, 45, 0));

	pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pDirectionalLight->SetDiffuseColor(Colors::WHITE);
	pDirectionalLight->SetSpecularColor(Colors::WHITE);
	pDirectionalLight->SetIntensity(1.0f);

	pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Rotate(glm::vec3(60, -45, 0));

	pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pDirectionalLight->SetDiffuseColor(Colors::WHITE);
	pDirectionalLight->SetSpecularColor(Colors::WHITE);
	pDirectionalLight->SetIntensity(1.0f);

	GameObject* pModel = ModelImporter::Import("models/Atrium.obj");

	pGameObject = GameObject::Instantiate();

	KeyBindings* pKeyBindings = KeyBindings::Instantiate(pGameObject);

	FirstPersonCameraController* pFirstPersonCameraController = FirstPersonCameraController::Instantiate(pGameObject);
	pFirstPersonCameraController->SetWalkSpeed(5.0f);
	pFirstPersonCameraController->SetTurnSpeed(180.0f);
	pFirstPersonCameraController->SetFlying(true);
}
