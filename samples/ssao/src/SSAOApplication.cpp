#include "SSAOApplication.h"
#include "Controls.h"

#include <FastCG/TextureImporter.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MathT.h>
#include <FastCG/FlyController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Colors.h>
#include <FastCG/Camera.h>

using namespace FastCG;

namespace
{
	void LoadModel()
	{
		auto *pDefaultMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Default Material",
																				 RenderingSystem::GetInstance()->FindShader("SolidColor")});

		auto *pModel = ModelImporter::Import("objs/armadillo.obj", pDefaultMaterial);
		if (pModel == nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Missing armadillo model");
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
		pTransform->SetPosition(glm::vec3(-center.x * scale, center.y * scale, -center.z * scale));
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

SSAOApplication::SSAOApplication() : Application({"ssao", 1024, 768, 60, false, RenderingPath::DEFERRED, {"ssao"}})
{
}

void SSAOApplication::OnStart()
{
	auto *pMainCameraGameObject = GameObject::Instantiate();
	pMainCameraGameObject->GetTransform()->SetPosition(glm::vec3{0, 0.25f, -1.5f});
	pMainCameraGameObject->GetTransform()->RotateAround(180, glm::vec3{0, 1, 0});

	Camera::Instantiate(pMainCameraGameObject, CameraSetupArgs{0.3f, 1000, 60, 1024 / (float)768}, ProjectionMode::PERSPECTIVE, true);

	auto *pFlyController = FlyController::Instantiate(pMainCameraGameObject);
	pFlyController->SetWalkSpeed(5);
	pFlyController->SetTurnSpeed(0.25f);

	LoadModel();
	CreateDirectionalLight();

	auto *pGeneralBehavioursGameObject = GameObject::Instantiate();
	Controls::Instantiate(pGeneralBehavioursGameObject);
}
