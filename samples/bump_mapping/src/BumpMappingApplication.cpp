#include "BumpMappingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"

#include <FastCG/TextureLoader.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/PointLight.h>
#include <FastCG/ModelLoader.h>
#include <FastCG/FlyController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Colors.h>
#include <FastCG/Camera.h>

using namespace FastCG;

namespace
{
	void CreateLights(std::vector<GameObject *> &rSceneLights)
	{
		auto *pSceneLights = GameObject::Instantiate("Scene Lights");

		auto *pLightGameObject = GameObject::Instantiate("Point Light 1");
		pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
		pLightGameObject->GetTransform()->SetPosition(glm::vec3(2, 2, 0));
		rSceneLights.emplace_back(pLightGameObject);

		auto *pPointLight = PointLight::Instantiate(pLightGameObject);
		pPointLight->SetDiffuseColor(Colors::WHITE);
		pPointLight->SetSpecularColor(Colors::WHITE);
		pPointLight->SetIntensity(1);
		pPointLight->SetQuadraticAttenuation(0.25f);

		pLightGameObject = GameObject::Instantiate("Point Light 2");
		pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
		pLightGameObject->GetTransform()->SetPosition(glm::vec3(-2, 2, 0));
		rSceneLights.emplace_back(pLightGameObject);

		pPointLight = PointLight::Instantiate(pLightGameObject);
		pPointLight->SetDiffuseColor(Colors::RED);
		pPointLight->SetSpecularColor(Colors::RED);
		pPointLight->SetIntensity(1);
		pPointLight->SetQuadraticAttenuation(0.25f);
	}

	void CreateGround()
	{
		auto *pGround = GameObject::Instantiate("Ground");

		auto *pGroundMesh = StandardGeometries::CreateXZPlane("Ground", 5, 5);

		auto *pGroundColorMap = TextureLoader::Load("textures/ground_d.png");
		auto *pGroupBumpMap = TextureLoader::Load("textures/ground_n.png");

		auto *pGroundMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Ground", RenderingSystem::GetInstance()->FindMaterialDefinition("OpaqueBumpedSpecular")});
		pGroundMaterial->SetTexture("uColorMap", pGroundColorMap);
		pGroundMaterial->SetConstant("uColorMapTiling", glm::vec2(4, 4));
		pGroundMaterial->SetTexture("uBumpMap", pGroupBumpMap);
		pGroundMaterial->SetConstant("uBumpMapTiling", glm::vec2(4, 4));
		pGroundMaterial->SetConstant("uDiffuseColor", Colors::WHITE);
		pGroundMaterial->SetConstant("uSpecularColor", Colors::WHITE);
		pGroundMaterial->SetConstant("uShininess", 30);

		Renderable::Instantiate(pGround, pGroundMaterial, pGroundMesh);
	}

	void LoadModel()
	{
		auto *pMissingMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Missing Material", RenderingSystem::GetInstance()->FindMaterialDefinition("OpaqueSolidColor")});
		pMissingMaterial->SetConstant("uDiffuseColor", Colors::PURPLE);

		auto *pModel = ModelLoader::Load("objs/doomsday.obj", pMissingMaterial);
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

}

BumpMappingApplication::BumpMappingApplication() : Application({"bump_mapping", 1024, 768, 60, false, {RenderingPath::FORWARD}, {{"bump_mapping"}}})
{
}

void BumpMappingApplication::OnStart()
{
	auto *pMainCameraGameObject = GameObject::Instantiate("Main Camera");
	pMainCameraGameObject->GetTransform()->SetPosition(glm::vec3(0, 0.5f, 1));

	Camera::Instantiate(pMainCameraGameObject, CameraSetupArgs{0.3f, 1000, 60, 1024 / (float)768}, ProjectionMode::PERSPECTIVE);

	auto *pFlyController = FlyController::Instantiate(pMainCameraGameObject);
	pFlyController->SetMoveSpeed(5);
	pFlyController->SetTurnSpeed(0.25f);

	LoadModel();
	CreateGround();
	std::vector<GameObject *> lights;
	CreateLights(lights);

	auto *pGeneralBehavioursGameObject = GameObject::Instantiate("General Behaviours");
	Controls::Instantiate(pGeneralBehavioursGameObject);
	auto *pLightsAnimator = LightsAnimator::Instantiate(pGeneralBehavioursGameObject);
	pLightsAnimator->SetLights(lights);
}
