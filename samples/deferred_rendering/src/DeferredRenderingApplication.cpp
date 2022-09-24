#include "DeferredRenderingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"
#include "SpheresAnimator.h"

#include <FastCG/TextureImporter.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/ShaderRegistry.h>
#include <FastCG/Random.h>
#include <FastCG/PointLight.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MeshRenderer.h>
#include <FastCG/MeshFilter.h>
#include <FastCG/MathT.h>
#include <FastCG/FlyCameraController.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Colors.h>

#include <vector>
#include <cstdint>

const uint32_t FLOOR_SIZE = 10;
const float SPHERE_RADIUS = FLOOR_SIZE * 0.025f;
const uint32_t NUMBER_OF_SPHERE_SLICES = 30;
const uint32_t LIGHT_GRID_WIDTH = 7;
const uint32_t LIGHT_GRID_DEPTH = 7;
const uint32_t LIGHT_GRID_SIZE = LIGHT_GRID_WIDTH * LIGHT_GRID_DEPTH;
const float WALK_SPEED = 20.0f;
const float TURN_SPEED = 20.0f;

DeferredRenderingApplication::DeferredRenderingApplication() :
	Application({ "deferred_rendering", 1024, 768, 60, RenderingPath::RP_DEFERRED_RENDERING })
{
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void DeferredRenderingApplication::OnStart()
{
	GetMainCamera()->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, FLOOR_SIZE * 0.5f, (float)FLOOR_SIZE));
	GetMainCamera()->GetGameObject()->GetTransform()->RotateAround(-20, glm::vec3(1, 0, 0));

	std::vector<PointLight*> pointLights;
	GameObject* pPointLightGameObject;
	PointLight* pPointLight;

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

	mpCheckersColorMapTexture = TextureImporter::Import("textures/CheckersColorMap.png");
	if (mpCheckersColorMapTexture == nullptr)
	{
		FASTCG_THROW_EXCEPTION(Exception, "Missing checkers texture");
	}

	mpFloorMaterial = std::make_shared<Material>(ShaderRegistry::Find("Specular"));
	mpFloorMaterial->SetTexture("colorMap", mpCheckersColorMapTexture);
	mpFloorMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpFloorMaterial->SetVec4("specularColor", Colors::WHITE);
	mpFloorMaterial->SetFloat("shininess", 5.0f);

	mpFloorMesh = StandardGeometries::CreateXZPlane("Floor", (float)FLOOR_SIZE, (float)FLOOR_SIZE, 1, 1, glm::vec3(0, 0, 0));

	auto* pFloorGameObject = GameObject::Instantiate();
	auto* pMeshRenderer = MeshRenderer::Instantiate(pFloorGameObject);
	pMeshRenderer->AddMesh(mpFloorMesh);
	auto* pMeshFilter = MeshFilter::Instantiate(pFloorGameObject);
	pMeshFilter->SetMaterial(mpFloorMaterial);

	mpSphereMaterial = std::make_shared<Material>(ShaderRegistry::Find("SolidColor"));
	mpSphereMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpSphereMaterial->SetVec4("specularColor", Colors::WHITE);
	mpSphereMaterial->SetFloat("shininess", 30.0f);

	mpSphereMesh = StandardGeometries::CreateSphere("Sphere", SPHERE_RADIUS, NUMBER_OF_SPHERE_SLICES);

	std::vector<GameObject*> spheres;
	for (float z = 0; z < FLOOR_SIZE; z += 1)
	{
		for (float x = 0; x < FLOOR_SIZE; x += 1)
		{
			auto* pSphereGameObject = GameObject::Instantiate();
			pMeshRenderer = MeshRenderer::Instantiate(pSphereGameObject);
			pMeshRenderer->AddMesh(mpSphereMesh);
			pMeshFilter = MeshFilter::Instantiate(pSphereGameObject);
			pMeshFilter->SetMaterial(mpSphereMaterial);
			pSphereGameObject->GetTransform()->SetPosition(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));
			spheres.emplace_back(pSphereGameObject);
		}
	}

	auto* pGeneralBehavioursGameObject = GameObject::Instantiate();

	auto* pLightsAnimator = LightsAnimator::Instantiate(pGeneralBehavioursGameObject);
	pLightsAnimator->SetLights(pointLights);

	auto* pControls = Controls::Instantiate(pGeneralBehavioursGameObject);

	auto* pFlyCameraController = FlyCameraController::Instantiate(pGeneralBehavioursGameObject);
	pFlyCameraController->SetWalkSpeed(WALK_SPEED);
	pFlyCameraController->SetTurnSpeed(TURN_SPEED);
}
