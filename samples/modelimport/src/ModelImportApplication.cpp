#include "ModelImportApplication.h"

#include <ModelImporter.h>
#include <MathT.h>

ModelImportApplication::ModelImportApplication() :
	Application("modelimport", 800, 600, 30)
{
	mGlobalAmbientLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mShowFPS = true;
}

void ModelImportApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, 0.0f, 1.0f));

	GameObject* pGameObject = ModelImporter::Import("models/OogieBoogie.obj");

	const AABB& boundingVolume = pGameObject->GetBoundingVolume();

	float scale = 0;
	scale = boundingVolume.max.x - boundingVolume.min.x;
	scale = MathF::Max(boundingVolume.max.y - boundingVolume.min.y, scale);
	scale = MathF::Max(boundingVolume.max.z - boundingVolume.min.z, scale);
	scale = 1.0f / scale;

	glm::mat4 worldTransform;
	worldTransform = glm::scale(worldTransform, glm::vec3(scale, scale, scale));
	worldTransform = glm::translate(worldTransform, -boundingVolume.center);
	pGameObject->GetTransform()->SetWorldTransform(worldTransform);
}
