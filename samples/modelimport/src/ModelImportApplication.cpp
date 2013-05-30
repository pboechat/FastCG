#include "ModelImportApplication.h"
#include "KeyBindings.h"

#include <DirectionalLight.h>
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
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));

	GameObject* pGameObject = GameObject::Instantiate();

	DirectionalLight* pDirectionalLight = DirectionalLight::Instantiate(pGameObject);
	pGameObject->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
	pDirectionalLight->SetAmbientColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectionalLight->SetDiffuseColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectionalLight->SetSpecularColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectionalLight->SetIntensity(1.0f);

	ModelImporter::LogToConsole();

	std::vector<GameObject*> models;
	pGameObject = ImportModelToScene("models/OogieBoogie.obj", ModelImporter::IS_QUALITY_GOOD | ModelImporter::IS_INVERT_TEXTURE_COORDINATE_V);
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/JackSkellington.obj", ModelImporter::IS_QUALITY_GOOD | ModelImporter::IS_INVERT_TEXTURE_COORDINATE_V);
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/UmberHulk.obj", ModelImporter::IS_QUALITY_GOOD | ModelImporter::IS_INVERT_TEXTURE_COORDINATE_V);
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Joker.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/GreenLantern.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Sinestro.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/BlackAdam.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Bane.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/SolomonGrundy.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Flash.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Doomsday.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Deathstroke.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/LexLuthor.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Nightwing.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Catwoman.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Superman.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/Batman.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/WonderWoman.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/GreenArrow.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/HarleyQuinn.obj");
	models.push_back(pGameObject);
	pGameObject = ImportModelToScene("models/WomanHead.obj", ModelImporter::IS_QUALITY_GOOD | ModelImporter::IS_INVERT_TEXTURE_COORDINATE_V);
	models.push_back(pGameObject);

	models[0]->SetActive(true);

	pGameObject = GameObject::Instantiate();

	KeyBindings* pKeyBindings = KeyBindings::Instantiate(pGameObject);
	pKeyBindings->SetModels(models);

	FirstPersonCameraController* pController = FirstPersonCameraController::Instantiate(pGameObject);
	pController->SetWalkSpeed(10.0f);
	pController->SetTurnSpeed(100.0f);
	pController->SetFlying(true);
}

GameObject* ModelImportApplication::ImportModelToScene(const std::string& rFileName, unsigned int importSettings)
{
	GameObject* pGameObject = ModelImporter::Import(rFileName, importSettings);

	const AABB& boundingVolume = pGameObject->GetBoundingVolume();
	Transform* pTransform = pGameObject->GetTransform();
	float scale = 0;
	scale = boundingVolume.max.x - boundingVolume.min.x;
	scale = MathF::Max(boundingVolume.max.y - boundingVolume.min.y, scale);
	scale = MathF::Max(boundingVolume.max.z - boundingVolume.min.z, scale);
	scale = 1.0f / scale;

	pTransform->ScaleLocal(glm::vec3(scale, scale, scale));
	pTransform->SetPosition(-boundingVolume.center * scale);

	pGameObject->SetActive(false);

	return pGameObject;
}
