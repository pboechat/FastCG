#include "DeferredRenderingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"

#include <FastCG/TextureLoader.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/Random.h>
#include <FastCG/PointLight.h>
#include <FastCG/ModelLoader.h>
#include <FastCG/MathT.h>
#include <FastCG/FlyController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Camera.h>
#include <FastCG/Colors.h>

#include <vector>
#include <cstdint>

using namespace FastCG;

namespace
{
	constexpr float GROUND_SIZE = 10;
	constexpr float SPHERE_RADIUS = GROUND_SIZE * 0.025f;
	constexpr uint32_t NUMBER_OF_SPHERE_SLICES = 30;
	constexpr uint32_t LIGHT_GRID_WIDTH = 7;
	constexpr uint32_t LIGHT_GRID_DEPTH = 7;
	constexpr uint32_t LIGHT_GRID_SIZE = LIGHT_GRID_WIDTH * LIGHT_GRID_DEPTH;

	void CreateGround()
	{
		auto *pCheckersColorMap = TextureLoader::Load("textures/checkers.png");
		if (pCheckersColorMap == nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Missing checkers texture");
		}

		auto *pFloorMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Ground", RenderingSystem::GetInstance()->FindMaterialDefinition("OpaqueSpecular")});
		pFloorMaterial->SetTexture("uColorMap", pCheckersColorMap);
		pFloorMaterial->SetConstant("uDiffuseColor", Colors::WHITE);
		pFloorMaterial->SetConstant("uSpecularColor", Colors::WHITE);
		pFloorMaterial->SetConstant("uShininess", 5);

		auto *pFloorMesh = StandardGeometries::CreateXZPlane("Ground", GROUND_SIZE, GROUND_SIZE, 1, 1, glm::vec3(0, 0, 0));

		auto *pFloorGameObject = GameObject::Instantiate("Floor");
		Renderable::Instantiate(pFloorGameObject, pFloorMaterial, pFloorMesh);
	}

	void CreateSpheres()
	{
		auto *pSphereMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Sphere", RenderingSystem::GetInstance()->FindMaterialDefinition("OpaqueSolidColor")});
		pSphereMaterial->SetConstant("uDiffuseColor", glm::vec4(1, 1, 1, 0.5f));
		pSphereMaterial->SetConstant("uSpecularColor", Colors::WHITE);
		pSphereMaterial->SetConstant("uShininess", 30);

		auto *pSphereMesh = StandardGeometries::CreateSphere("Sphere", SPHERE_RADIUS, NUMBER_OF_SPHERE_SLICES);

		for (float z = 0; z < GROUND_SIZE; z += 1)
		{
			for (float x = 0; x < GROUND_SIZE; x += 1)
			{
				auto *pSphereGameObject = GameObject::Instantiate("Sphere");
				pSphereGameObject->GetTransform()->SetPosition(glm::vec3(-(GROUND_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(GROUND_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));
				Renderable::Instantiate(pSphereGameObject, pSphereMaterial, pSphereMesh);
			}
		}
	}

	void CreateLights(std::vector<PointLight *> &rSceneLights)
	{
		float depthIncrement = GROUND_SIZE / (float)LIGHT_GRID_DEPTH;
		float horizontalIncrement = GROUND_SIZE / (float)LIGHT_GRID_WIDTH;
		float intensity = 0.2f;
		Random::Seed(0);
		int i = 0;
		for (float z = 1; z < GROUND_SIZE; z += depthIncrement)
		{
			for (float x = 1; x < GROUND_SIZE; x += horizontalIncrement)
			{
				auto *pPointLightGameObject = GameObject::Instantiate((std::string("Point Light ") + std::to_string(i++)).c_str());
				pPointLightGameObject->GetTransform()->SetPosition(glm::vec3(-(GROUND_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS * 2.5f, -(GROUND_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));

				auto *pPointLight = PointLight::Instantiate(pPointLightGameObject);
				auto color = Colors::COMMON_LIGHT_COLORS[Random::Range<size_t>(0, Colors::NUMBER_OF_COMMON_LIGHT_COLORS)];
				pPointLight->SetDiffuseColor(color);
				pPointLight->SetSpecularColor(color);
				pPointLight->SetIntensity(intensity);

				rSceneLights.emplace_back(pPointLight);
			}
		}
	}

}

DeferredRenderingApplication::DeferredRenderingApplication() : Application({"deferred_rendering", 1024, 768, 60, false, {RenderingPath::DEFERRED}, {{"deferred_rendering"}}})
{
}

void DeferredRenderingApplication::OnStart()
{
	auto *pMainCameraGameObject = GameObject::Instantiate("Main Camera");
	pMainCameraGameObject->GetTransform()->SetPosition(glm::vec3(0, GROUND_SIZE * 0.5f, GROUND_SIZE));
	pMainCameraGameObject->GetTransform()->RotateAround(-20, glm::vec3(1, 0, 0));

	Camera::Instantiate(pMainCameraGameObject, CameraSetupArgs{0.3f, 1000, 60, 1024 / (float)768}, ProjectionMode::PERSPECTIVE);

	auto *pFlyController = FlyController::Instantiate(pMainCameraGameObject);
	pFlyController->SetMoveSpeed(5);
	pFlyController->SetTurnSpeed(0.25f);

	CreateGround();
	CreateSpheres();

	std::vector<PointLight *> lights;
	CreateLights(lights);

	auto *pGeneralBehavioursGameObject = GameObject::Instantiate("General Behaviours");
	auto *pLightsAnimator = LightsAnimator::Instantiate(pGeneralBehavioursGameObject);
	pLightsAnimator->SetLights(lights);
	Controls::Instantiate(pGeneralBehavioursGameObject);
}
