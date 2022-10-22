#include "DeferredRenderingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"
#include "SpheresAnimator.h"

#include <FastCG/TextureImporter.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/Renderable.h>
#include <FastCG/Random.h>
#include <FastCG/PointLight.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MathT.h>
#include <FastCG/FlyCameraController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Camera.h>
#include <FastCG/Colors.h>

#include <vector>
#include <cstdint>

using namespace FastCG;

constexpr uint32_t FLOOR_SIZE = 10;
constexpr float SPHERE_RADIUS = FLOOR_SIZE * 0.025f;
constexpr uint32_t NUMBER_OF_SPHERE_SLICES = 30;
constexpr uint32_t LIGHT_GRID_WIDTH = 7;
constexpr uint32_t LIGHT_GRID_DEPTH = 7;
constexpr uint32_t LIGHT_GRID_SIZE = LIGHT_GRID_WIDTH * LIGHT_GRID_DEPTH;
constexpr float WALK_SPEED = 20.0f;
constexpr float TURN_SPEED = 20.0f;

DeferredRenderingApplication::DeferredRenderingApplication() : Application({"deferred_rendering", 1024, 768, 60, RenderingPath::RP_DEFERRED_RENDERING, {"deferred_rendering"}})
{
}

void DeferredRenderingApplication::OnStart()
{
	GetMainCamera()->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, FLOOR_SIZE * 0.5f, (float)FLOOR_SIZE));
	GetMainCamera()->GetGameObject()->GetTransform()->RotateAround(-20, glm::vec3(1, 0, 0));

	std::vector<PointLight *> pointLights;
	GameObject *pPointLightGameObject;
	PointLight *pPointLight;

	float depthIncrement = FLOOR_SIZE / (float)LIGHT_GRID_DEPTH;
	float horizontalIncrement = FLOOR_SIZE / (float)LIGHT_GRID_WIDTH;
	float intensity = 0.2f;

	for (float z = 1; z < FLOOR_SIZE; z += depthIncrement)
	{
		for (float x = 1; x < FLOOR_SIZE; x += horizontalIncrement)
		{
			pPointLightGameObject = GameObject::Instantiate();
			pPointLightGameObject->GetTransform()->SetPosition(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS * 2.5f, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));

			pPointLight = PointLight::Instantiate(pPointLightGameObject);
			auto color = Colors::COMMON_LIGHT_COLORS[Random::Range<size_t>(0, Colors::NUMBER_OF_COMMON_LIGHT_COLORS)];
			pPointLight->SetDiffuseColor(color);
			pPointLight->SetSpecularColor(color);
			pPointLight->SetIntensity(intensity);

			pointLights.emplace_back(pPointLight);
		}
	}

	mpCheckersColorMap = TextureImporter::Import("textures/checkers.png");
	if (mpCheckersColorMap == nullptr)
	{
		FASTCG_THROW_EXCEPTION(Exception, "Missing checkers texture");
	}

	mpFloorMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Floor", RenderingSystem::GetInstance()->FindShader("Specular")});
	mpFloorMaterial->SetColorMap(mpCheckersColorMap);
	mpFloorMaterial->SetDiffuseColor(Colors::WHITE);
	mpFloorMaterial->SetSpecularColor(Colors::WHITE);
	mpFloorMaterial->SetShininess(5.0f);

	mpFloorMesh = StandardGeometries::CreateXZPlane("Floor", (float)FLOOR_SIZE, (float)FLOOR_SIZE, 1, 1, glm::vec3(0, 0, 0));

	auto *pFloorGameObject = GameObject::Instantiate();
	auto *pRenderable = Renderable::Instantiate(pFloorGameObject, mpFloorMaterial, mpFloorMesh);

	mpSphereMaterial = RenderingSystem::GetInstance()->CreateMaterial({"Sphere", RenderingSystem::GetInstance()->FindShader("SolidColor")});
	mpSphereMaterial->SetDiffuseColor(Colors::WHITE);
	mpSphereMaterial->SetSpecularColor(Colors::WHITE);
	mpSphereMaterial->SetShininess(30.0f);

	mpSphereMesh = StandardGeometries::CreateSphere("Sphere", SPHERE_RADIUS, NUMBER_OF_SPHERE_SLICES);

	std::vector<GameObject *> spheres;
	for (float z = 0; z < FLOOR_SIZE; z += 1)
	{
		for (float x = 0; x < FLOOR_SIZE; x += 1)
		{
			auto *pSphereGameObject = GameObject::Instantiate();
			pRenderable = Renderable::Instantiate(pSphereGameObject, mpSphereMaterial, mpSphereMesh);
			pSphereGameObject->GetTransform()->SetPosition(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));
			spheres.emplace_back(pSphereGameObject);
		}
	}

	auto *pGeneralBehavioursGameObject = GameObject::Instantiate();
	auto *pLightsAnimator = LightsAnimator::Instantiate(pGeneralBehavioursGameObject);
	pLightsAnimator->SetLights(pointLights);
	auto *pControls = Controls::Instantiate(pGeneralBehavioursGameObject);
	auto *pFlyCameraController = FlyCameraController::Instantiate(pGeneralBehavioursGameObject);
	pFlyCameraController->SetWalkSpeed(WALK_SPEED);
	pFlyCameraController->SetTurnSpeed(TURN_SPEED);
}
