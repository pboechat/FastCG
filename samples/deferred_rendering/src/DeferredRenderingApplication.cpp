#include "DeferredRenderingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"

#include <FastCG/TextureImporter.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/Random.h>
#include <FastCG/PointLight.h>
#include <FastCG/ModelImporter.h>
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
	constexpr uint32_t FLOOR_SIZE = 10;
	constexpr float SPHERE_RADIUS = FLOOR_SIZE * 0.025f;
	constexpr uint32_t NUMBER_OF_SPHERE_SLICES = 30;
	constexpr uint32_t LIGHT_GRID_WIDTH = 7;
	constexpr uint32_t LIGHT_GRID_DEPTH = 7;
	constexpr uint32_t LIGHT_GRID_SIZE = LIGHT_GRID_WIDTH * LIGHT_GRID_DEPTH;

	void CreateFloor()
	{
		auto *pCheckersColorMap = TextureImporter::Import("textures/checkers.png");
		if (pCheckersColorMap == nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "Missing checkers texture");
		}

		auto *pFloorMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Floor", RenderingSystem::GetInstance()->FindShader("Specular")});
		pFloorMaterial->SetColorMap(pCheckersColorMap);
		pFloorMaterial->SetDiffuseColor(Colors::WHITE);
		pFloorMaterial->SetSpecularColor(Colors::WHITE);
		pFloorMaterial->SetShininess(5.0f);

		auto *pFloorMesh = StandardGeometries::CreateXZPlane("Floor", (float)FLOOR_SIZE, (float)FLOOR_SIZE, 1, 1, glm::vec3(0, 0, 0));

		auto *pFloorGameObject = GameObject::Instantiate();
		Renderable::Instantiate(pFloorGameObject, pFloorMaterial, pFloorMesh);
	}

	void CreateSpheres()
	{
		auto *pSphereMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Sphere", RenderingSystem::GetInstance()->FindShader("SolidColor")});
		pSphereMaterial->SetDiffuseColor(Colors::WHITE);
		pSphereMaterial->SetSpecularColor(Colors::WHITE);
		pSphereMaterial->SetShininess(30.0f);

		auto *pSphereMesh = StandardGeometries::CreateSphere("Sphere", SPHERE_RADIUS, NUMBER_OF_SPHERE_SLICES);

		for (float z = 0; z < FLOOR_SIZE; z += 1)
		{
			for (float x = 0; x < FLOOR_SIZE; x += 1)
			{
				auto *pSphereGameObject = GameObject::Instantiate();
				pSphereGameObject->GetTransform()->SetPosition(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));
				Renderable::Instantiate(pSphereGameObject, pSphereMaterial, pSphereMesh);
			}
		}
	}

	void CreateLights(std::vector<PointLight *> &rSceneLights)
	{
		float depthIncrement = FLOOR_SIZE / (float)LIGHT_GRID_DEPTH;
		float horizontalIncrement = FLOOR_SIZE / (float)LIGHT_GRID_WIDTH;
		float intensity = 0.2f;
		for (float z = 1; z < FLOOR_SIZE; z += depthIncrement)
		{
			for (float x = 1; x < FLOOR_SIZE; x += horizontalIncrement)
			{
				auto *pPointLightGameObject = GameObject::Instantiate();
				pPointLightGameObject->GetTransform()->SetPosition(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS * 2.5f, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));

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

DeferredRenderingApplication::DeferredRenderingApplication() : Application({"deferred_rendering", 1024, 768, 60, RenderingPath::DEFERRED_RENDERING, {"deferred_rendering"}})
{
}

void DeferredRenderingApplication::OnStart()
{
	auto *pMainCameraGameObject = GameObject::Instantiate();
	pMainCameraGameObject->GetTransform()->SetPosition(glm::vec3(0, FLOOR_SIZE * 0.5f, (float)FLOOR_SIZE));
	pMainCameraGameObject->GetTransform()->RotateAround(-20, glm::vec3(1, 0, 0));

	Camera::Instantiate(pMainCameraGameObject);

	auto *pFlyController = FlyController::Instantiate(pMainCameraGameObject);
	pFlyController->SetWalkSpeed(5);
	pFlyController->SetTurnSpeed(0.25f);

	CreateFloor();
	CreateSpheres();

	std::vector<PointLight *> lights;
	CreateLights(lights);

	auto *pGeneralBehavioursGameObject = GameObject::Instantiate();
	auto *pLightsAnimator = LightsAnimator::Instantiate(pGeneralBehavioursGameObject);
	pLightsAnimator->SetLights(lights);
	Controls::Instantiate(pGeneralBehavioursGameObject);
}
