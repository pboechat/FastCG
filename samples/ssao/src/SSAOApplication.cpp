#include "SSAOApplication.h"
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

SSAOApplication::SSAOApplication() :
	Application({ "ssao", 1024, 768, 60, RenderingPath::RP_DEFERRED_RENDERING })
{
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void SSAOApplication::OnStart()
{
	GetMainCamera()->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, 2, 0));
	GetMainCamera()->SetSSAOEnabled(true);

	auto* pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Rotate(glm::vec3(60, 90, 0));

	auto* pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pDirectionalLight->SetDiffuseColor(Colors::WHITE);
	pDirectionalLight->SetSpecularColor(Colors::WHITE);
	pDirectionalLight->SetIntensity(0.5f);

	pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Rotate(glm::vec3(60, -90, 0));

	pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pDirectionalLight->SetDiffuseColor(Colors::WHITE);
	pDirectionalLight->SetSpecularColor(Colors::WHITE);
	pDirectionalLight->SetIntensity(0.5f);

	pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Rotate(glm::vec3(60, 45, 0));

	pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pDirectionalLight->SetDiffuseColor(Colors::WHITE);
	pDirectionalLight->SetSpecularColor(Colors::WHITE);
	pDirectionalLight->SetIntensity(0.5f);

	pGameObject = GameObject::Instantiate();
	pGameObject->GetTransform()->Rotate(glm::vec3(60, -45, 0));

	pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pDirectionalLight->SetDiffuseColor(Colors::WHITE);
	pDirectionalLight->SetSpecularColor(Colors::WHITE);
	pDirectionalLight->SetIntensity(0.5f);

	auto* pModel = ModelImporter::Import("objs/Atrium.obj", (ModelImporterOptions)ModelImporterOption::MIO_EXPORT_OPTIMIZED_MODEL_FILE);

	pGameObject = GameObject::Instantiate();

	auto* pControls = Controls::Instantiate(pGameObject);

	auto* pFlyCameraController = FlyCameraController::Instantiate(pGameObject);
	pFlyCameraController->SetWalkSpeed(5.0f);
	pFlyCameraController->SetTurnSpeed(180.0f);
}
