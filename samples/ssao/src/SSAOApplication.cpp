#include "SSAOApplication.h"
#include "Controls.h"

#include <FastCG/TextureImporter.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/Renderable.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MathT.h>
#include <FastCG/FlyCameraController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Colors.h>
#include <FastCG/Camera.h>

using namespace FastCG;

namespace
{
	void LoadModel()
	{
		auto *pModel = ModelImporter::Import("objs/doomsday.obj");
		if (pModel == nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Missing doomsday model");
		}

		const auto &bounds = pModel->GetBounds();
		auto *pTransform = pModel->GetTransform();
		float scale = 0;
		scale = bounds.max.x - bounds.min.x;
		scale = MathF::Max(bounds.max.y - bounds.min.y, scale);
		scale = MathF::Max(bounds.max.z - bounds.min.z, scale);
		scale = 1.0f / scale;

		pTransform->SetScale(glm::vec3(scale, scale, scale));
		auto center = bounds.getCenter();
		pTransform->SetPosition(glm::vec3(-center.x * scale, 0, -center.z * scale));
	}

	void CreateGround()
	{
		auto *pGroundMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Ground", RenderingSystem::GetInstance()->FindShader("SolidColor")});
		pGroundMaterial->SetDiffuseColor(Colors::WHITE);
		pGroundMaterial->SetSpecularColor(Colors::WHITE);
		pGroundMaterial->SetShininess(1);

		auto *pGroundGameObject = GameObject::Instantiate();
		auto *pGroundMesh = StandardGeometries::CreateXZPlane("Ground", 5, 5);
		Renderable::Instantiate(pGroundGameObject, pGroundMaterial, pGroundMesh);
	}

	void CreateDirectionalLight()
	{
		auto *pDirectionalLightGameObject = GameObject::Instantiate();
		auto *pDirectionalLight = DirectionalLight::Instantiate(pDirectionalLightGameObject);
		pDirectionalLight->SetDiffuseColor(Colors::WHITE);
		pDirectionalLight->SetIntensity(1);

		pDirectionalLightGameObject->GetTransform()->SetPosition(glm::vec3(1, 1, 0));
	}

}

SSAOApplication::SSAOApplication() : Application({"ssao", 1024, 768, 60, RenderingPath::DEFERRED_RENDERING, {"ssao"}})
{
}

void SSAOApplication::OnStart()
{
	GetMainCamera()->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, 0.5f, 1));
	GetMainCamera()->SetSSAOEnabled(true);

	LoadModel();

	CreateGround();

	CreateDirectionalLight();

	auto *pGeneralBehavioursGameObject = GameObject::Instantiate();
	auto *pControls = Controls::Instantiate(pGeneralBehavioursGameObject);
	auto *pFlyCameraController = FlyCameraController::Instantiate(pGeneralBehavioursGameObject);
	pFlyCameraController->SetWalkSpeed(5);
	pFlyCameraController->SetTurnSpeed(0.25f);
}
