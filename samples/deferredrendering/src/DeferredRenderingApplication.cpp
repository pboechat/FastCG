#include "DeferredRenderingApplication.h"
#include "KeyBindings.h"
#include "LightsAnimator.h"
#include "SpheresAnimator.h"

#include <Random.h>
#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <DirectionalLight.h>
#include <PointLight.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <ModelImporter.h>
#include <FirstPersonCameraController.h>
#include <TextureImporter.h>
#include <Colors.h>
#include <MathT.h>

#include <vector>

const unsigned int DeferredRenderingApplication::FLOOR_SIZE = 10;
const float DeferredRenderingApplication::SPHERE_RADIUS = (FLOOR_SIZE * 0.025f);
const unsigned int DeferredRenderingApplication::NUMBER_OF_SPHERE_SLICES = 30;
const unsigned int DeferredRenderingApplication::LIGHT_GRID_WIDTH = 7;
const unsigned int DeferredRenderingApplication::LIGHT_GRID_DEPTH = 7;
const unsigned int DeferredRenderingApplication::LIGHT_GRID_SIZE = LIGHT_GRID_WIDTH * LIGHT_GRID_DEPTH;
const float DeferredRenderingApplication::WALK_SPEED = 20.0f;
const float DeferredRenderingApplication::TURN_SPEED = 100.0f;

DeferredRenderingApplication::DeferredRenderingApplication() :
#ifdef FIXED_FUNCTION_PIPELINE
	Application("deferredrendering", 1024, 768, 30),
#else
	Application("deferredrendering", 1024, 768, 30, true),
#endif
	mpCheckersColorMapTexture(0),
	mpFloorMaterial(0),
	mpFloorMesh(0),
	mpSphereMesh(0),
	mpSphereMaterial(0)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void DeferredRenderingApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.0f, FLOOR_SIZE * 0.5f, (float)FLOOR_SIZE));
	mpMainCamera->GetGameObject()->GetTransform()->RotateAroundLocal(-20.0f, glm::vec3(1.0f, 0.0f, 0.0f));

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
			pPointLightGameObject->GetTransform()->SetPosition(glm::vec3(-(FLOOR_SIZE * 0.5f - (2.0f * SPHERE_RADIUS)) + x, SPHERE_RADIUS * 2.5f, -(FLOOR_SIZE * 0.5f - (2.0f * SPHERE_RADIUS)) + z));

			pPointLight = PointLight::Instantiate(pPointLightGameObject);
			glm::vec4 color = Colors::COMMON_LIGHT_COLORS[Random::Range(0, Colors::NUMBER_OF_COMMON_LIGHT_COLORS)];
			pPointLight->SetDiffuseColor(color);
			pPointLight->SetSpecularColor(color);
			pPointLight->SetIntensity(intensity);

			pointLights.push_back(pPointLight);
		}
	}

	mpCheckersColorMapTexture = TextureImporter::Import("textures/CheckersColorMap.png");

#ifdef FIXED_FUNCTION_PIPELINE
	mpFloorMaterial = new Material();
	mpFloorMaterial->SetTexture(mpCheckersColorMapTexture);
	mpFloorMaterial->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	mpFloorMaterial->SetShininess(5.0f);
#else
	mpFloorMaterial = new Material(ShaderRegistry::Find("Specular"));
	mpFloorMaterial->SetTexture("colorMap", mpCheckersColorMapTexture);
	mpFloorMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpFloorMaterial->SetVec4("specularColor", Colors::WHITE);
	mpFloorMaterial->SetFloat("shininess", 5.0f);
#endif

	mpFloorMesh = StandardGeometries::CreateXZPlane((float)FLOOR_SIZE, (float)FLOOR_SIZE, 1, 1, glm::vec3(0, 0, 0));

	GameObject* pFloorGameObject = GameObject::Instantiate();
	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pFloorGameObject);
	pMeshRenderer->AddMesh(mpFloorMesh);
	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pFloorGameObject);
	pMeshFilter->SetMaterial(mpFloorMaterial);

#ifdef FIXED_FUNCTION_PIPELINE
	mpSphereMaterial = new Material();
	mpSphereMaterial->SetShininess(1.0f);
#else
	mpSphereMaterial = new Material(ShaderRegistry::Find("SolidColor"));
	mpSphereMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpSphereMaterial->SetVec4("specularColor", Colors::WHITE);
	mpSphereMaterial->SetFloat("shininess", 30.0f);
#endif

	mpSphereMesh = StandardGeometries::CreateSphere(SPHERE_RADIUS, NUMBER_OF_SPHERE_SLICES);

	std::vector<GameObject*> spheres;
	for (float z = 0; z < FLOOR_SIZE; z += 1.0f)
	{
		for (float x = 0; x < FLOOR_SIZE; x += 1.0f)
		{
			GameObject* pSphereGameObject = GameObject::Instantiate();
			pMeshRenderer = MeshRenderer::Instantiate(pSphereGameObject);
			pMeshRenderer->AddMesh(mpSphereMesh);
			pMeshFilter = MeshFilter::Instantiate(pSphereGameObject);
			pMeshFilter->SetMaterial(mpSphereMaterial);
			pSphereGameObject->GetTransform()->SetPosition(glm::vec3(-(FLOOR_SIZE * 0.5f - (2.0f * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(FLOOR_SIZE * 0.5f - (2.0f * SPHERE_RADIUS)) + z));
			spheres.push_back(pSphereGameObject);
		}
	}

	GameObject* pGeneralBehavioursGameObject = GameObject::Instantiate();

	LightsAnimator* pLightsAnimator = LightsAnimator::Instantiate(pGeneralBehavioursGameObject);
	pLightsAnimator->SetLights(pointLights);

	KeyBindings* pKeyBindings = KeyBindings::Instantiate(pGeneralBehavioursGameObject);

	FirstPersonCameraController* pFirstPersonCameraController = FirstPersonCameraController::Instantiate(pGeneralBehavioursGameObject);
	pFirstPersonCameraController->SetWalkSpeed(WALK_SPEED);
	pFirstPersonCameraController->SetTurnSpeed(TURN_SPEED);
	pFirstPersonCameraController->SetFlying(true);
}

void DeferredRenderingApplication::OnEnd()
{
	if (mpCheckersColorMapTexture != 0)
	{
		delete mpCheckersColorMapTexture;
		mpCheckersColorMapTexture = 0;
	}

	if (mpFloorMaterial != 0)
	{
		delete mpFloorMaterial;
		mpFloorMaterial = 0;
	}

	if (mpFloorMesh != 0)
	{
		delete mpFloorMesh;
		mpFloorMesh = 0;
	}

	if (mpSphereMaterial != 0)
	{
		delete mpSphereMaterial;
	}

	if (mpSphereMesh != 0)
	{
		delete mpSphereMesh;
		mpSphereMesh = 0;
	}
}
