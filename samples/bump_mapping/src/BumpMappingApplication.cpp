#include "BumpMappingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"

#include <FastCG/TextureImporter.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/PointLight.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/FlyCameraController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Colors.h>
#include <FastCG/Camera.h>

using namespace FastCG;

namespace
{
	void CreateSceneLights(std::vector<GameObject *> &rSceneLights)
	{
		auto *pSceneLights = GameObject::Instantiate();

		auto *pLightGameObject = GameObject::Instantiate();
		pLightGameObject->GetTransform()->SetParent(pSceneLights->GetTransform());
		pLightGameObject->GetTransform()->SetPosition(glm::vec3(2, 2, 0));
		rSceneLights.emplace_back(pLightGameObject);

		auto *pPointLight = PointLight::Instantiate(pLightGameObject);
		pPointLight->SetDiffuseColor(Colors::WHITE);
		pPointLight->SetSpecularColor(Colors::WHITE);
		pPointLight->SetIntensity(1);
		pPointLight->SetQuadraticAttenuation(0.25f);

		pLightGameObject = GameObject::Instantiate();
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
		auto *pGround = GameObject::Instantiate();

		auto *pGroundMesh = StandardGeometries::CreateXZPlane("Ground", 5, 5);

		auto *pGroundColorMap = TextureImporter::Import("textures/ground_d.png");
		auto *pGroupBumpMap = TextureImporter::Import("textures/ground_n.png");

		auto *pGroundMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Ground", RenderingSystem::GetInstance()->FindShader("BumpedSpecular")});
		pGroundMaterial->SetColorMap(pGroundColorMap);
		pGroundMaterial->SetColorMapTiling(glm::vec2(4, 4));
		pGroundMaterial->SetBumpMap(pGroupBumpMap);
		pGroundMaterial->SetBumpMapTiling(glm::vec2(4, 4));
		pGroundMaterial->SetDiffuseColor(Colors::WHITE);
		pGroundMaterial->SetSpecularColor(Colors::WHITE);
		pGroundMaterial->SetShininess(30);

		Renderable::Instantiate(pGround, pGroundMaterial, pGroundMesh);
	}

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

}

BumpMappingApplication::BumpMappingApplication() : Application({"bump_mapping", 1024, 768, UNLOCKED_FRAMERATE, RenderingPath::FORWARD_RENDERING, {"bump_mapping"}})
{
}

void BumpMappingApplication::OnStart()
{
	GetMainCamera()->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, 0.5f, 1));

	std::vector<GameObject *> sceneLights;
	CreateSceneLights(sceneLights);

	LoadModel();

	CreateGround();

	auto *pGeneralBehavioursGameObject = GameObject::Instantiate();
	Controls::Instantiate(pGeneralBehavioursGameObject);
	auto *pLightsAnimator = LightsAnimator::Instantiate(pGeneralBehavioursGameObject);
	pLightsAnimator->SetLights(sceneLights);
	auto *pPlayerGameObject = GameObject::Instantiate();
	auto *pFlyCameraController = FlyCameraController::Instantiate(pPlayerGameObject);
	pFlyCameraController->SetWalkSpeed(5);
	pFlyCameraController->SetTurnSpeed(5);
}
