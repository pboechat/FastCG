#include "BumpMappingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"

#include <FastCG/World/FlyController.h>
#include <FastCG/World/ComponentRegistry.h>
#include <FastCG/Rendering/StandardGeometries.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/Rendering/PointLight.h>
#include <FastCG/Rendering/ModelLoader.h>
#include <FastCG/Rendering/MaterialDefinitionRegistry.h>
#include <FastCG/Rendering/DirectionalLight.h>
#include <FastCG/Rendering/Camera.h>
#include <FastCG/Graphics/TextureLoader.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Core/Colors.h>
#include <FastCG/Assets/AssetSystem.h>

#include <memory>

using namespace FastCG;

namespace
{
	void CreateLights(Transform *pParent)
	{
		auto *pLightsGameObject = GameObject::Instantiate("Lights");
		pLightsGameObject->GetTransform()->SetParent(pParent);

		auto *pLightGameObject = GameObject::Instantiate("Light 1");
		pLightGameObject->GetTransform()->SetParent(pLightsGameObject->GetTransform());
		pLightGameObject->GetTransform()->SetPosition(glm::vec3(2, 2, 0));
		pLightGameObject->GetTransform()->SetParent(pLightsGameObject->GetTransform());

		auto *pLight = PointLight::Instantiate(pLightGameObject);
		pLight->SetDiffuseColor(Colors::WHITE);
		pLight->SetSpecularColor(Colors::WHITE);
		pLight->SetIntensity(1);
		pLight->SetQuadraticAttenuation(0.25f);

		pLightGameObject = GameObject::Instantiate("Light 2");
		pLightGameObject->GetTransform()->SetParent(pLightsGameObject->GetTransform());
		pLightGameObject->GetTransform()->SetPosition(glm::vec3(-2, 2, 0));
		pLightGameObject->GetTransform()->SetParent(pLightsGameObject->GetTransform());

		pLight = PointLight::Instantiate(pLightGameObject);
		pLight->SetDiffuseColor(Colors::RED);
		pLight->SetSpecularColor(Colors::RED);
		pLight->SetIntensity(1);
		pLight->SetQuadraticAttenuation(0.25f);
	}

	void CreateGround(Transform *pParent)
	{
		auto *pGroundGameObject = GameObject::Instantiate("Ground");
		pGroundGameObject->GetTransform()->SetParent(pParent);

		auto pGroundMesh = StandardGeometries::CreateXZPlane("Ground", 5, 5);

		auto *pGroundColorMap = TextureLoader::Load(AssetSystem::GetInstance()->Resolve("textures/ground_d.png"));
		auto *pGroupBumpMap = TextureLoader::Load(AssetSystem::GetInstance()->Resolve("textures/ground_n.png"));

		auto pGroundMaterial = std::make_unique<Material>(MaterialArgs{"Ground", MaterialDefinitionRegistry::GetInstance()->GetMaterialDefinition("OpaqueBumpedSpecular")});
		pGroundMaterial->SetTexture("uColorMap", pGroundColorMap);
		pGroundMaterial->SetConstant("uColorMapTiling", glm::vec2(4, 4));
		pGroundMaterial->SetTexture("uBumpMap", pGroupBumpMap);
		pGroundMaterial->SetConstant("uBumpMapTiling", glm::vec2(4, 4));
		pGroundMaterial->SetConstant("uDiffuseColor", Colors::WHITE);
		pGroundMaterial->SetConstant("uSpecularColor", Colors::WHITE);
		pGroundMaterial->SetConstant("uShininess", 30);

		Renderable::Instantiate(pGroundGameObject, std::move(pGroundMaterial), std::move(pGroundMesh));
	}

	void LoadModel(Transform *pParent)
	{
		auto pMissingMaterial = std::make_shared<Material>(MaterialArgs{"Missing Material", MaterialDefinitionRegistry::GetInstance()->GetMaterialDefinition("OpaqueSolidColor")});
		pMissingMaterial->SetConstant("uDiffuseColor", Colors::PURPLE);

		auto *pModelGameObject = ModelLoader::Load(AssetSystem::GetInstance()->Resolve("objs/doomsday.obj"), pMissingMaterial);
		if (pModelGameObject == nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Missing doomsday model");
		}

		const auto &bounds = pModelGameObject->GetBounds();
		auto *pTransform = pModelGameObject->GetTransform();
		float scale = 0;
		scale = bounds.max.x - bounds.min.x;
		scale = MathF::Max(bounds.max.y - bounds.min.y, scale);
		scale = MathF::Max(bounds.max.z - bounds.min.z, scale);
		scale = 1.0f / scale;

		pTransform->SetScale(glm::vec3(scale, scale, scale));
		auto center = bounds.getCenter();
		pTransform->SetPosition(glm::vec3(-center.x * scale, 0, -center.z * scale));

		pTransform->SetParent(pParent);
	}

}

BumpMappingApplication::BumpMappingApplication() : Application({"bump_mapping", 1024, 768, 60, false, {RenderingPath::FORWARD}, {{"bump_mapping"}}})
{
	ComponentRegistry::RegisterComponent<Controls>();
	ComponentRegistry::RegisterComponent<LightsAnimator>();
}

void BumpMappingApplication::OnStart()
{
	auto *pSceneRootGameObject = GameObject::Instantiate("Scene Root");
	auto *pParent = pSceneRootGameObject->GetTransform();

	auto *pMainCameraGameObject = GameObject::Instantiate("Main Camera");
	auto *pTransform = pMainCameraGameObject->GetTransform();
	pTransform->SetPosition(glm::vec3(0, 0.5f, 1));
	pTransform->SetParent(pParent);

	Camera::Instantiate(pMainCameraGameObject, CameraSetupArgs{0.3f, 1000, 60, 1024 / (float)768}, ProjectionMode::PERSPECTIVE);

	auto *pFlyController = FlyController::Instantiate(pMainCameraGameObject);
	pFlyController->SetMoveSpeed(5);
	pFlyController->SetTurnSpeed(0.25f);

	LoadModel(pParent);
	CreateGround(pParent);
	CreateLights(pParent);

	auto *pGeneralBehavioursGameObject = GameObject::Instantiate("General Behaviours");
	pGeneralBehavioursGameObject->GetTransform()->SetParent(pParent);
	Controls::Instantiate(pGeneralBehavioursGameObject);
	LightsAnimator::Instantiate(pGeneralBehavioursGameObject);
}
