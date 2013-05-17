#include "DeferredShadingApplication.h"
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
#include <TextureUtils.h>
#include <MathT.h>

#include <vector>

const unsigned int DeferredShadingApplication::FLOOR_SIZE = 100;
const float DeferredShadingApplication::SPHERE_RADIUS = (FLOOR_SIZE * 0.025f);
const unsigned int DeferredShadingApplication::NUMBER_OF_SPHERE_SLICES = 30;
const unsigned int DeferredShadingApplication::NUMBER_OF_LIGHTS = 50;
const float DeferredShadingApplication::LIGHT_DISTANCE = FLOOR_SIZE * 0.5f;
const float DeferredShadingApplication::WALK_SPEED = 20.0f;
const float DeferredShadingApplication::TURN_SPEED = 100.0f;

DeferredShadingApplication::DeferredShadingApplication() :
#ifdef USE_PROGRAMMABLE_PIPELINE
	Application("deferredshading", 1024, 768, true),
#else
	Application("deferredshading", 1024, 768),
#endif
	mpCheckersColorMapTexture(0),
	mpFloorMaterial(0),
	mpFloorMesh(0),
	mpSphereColorMapTexture(0),
	mpSphereMesh(0),
	mpSphereMaterial(0)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void DeferredShadingApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, FLOOR_SIZE * 0.5f, (float)FLOOR_SIZE));
	mpMainCamera->GetGameObject()->GetTransform()->RotateAroundLocal(-20.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	GameObject* pDirectionalLightGameObject = GameObject::Instantiate();
	DirectionalLight* pDirectionalLight = DirectionalLight::Instantiate(pDirectionalLightGameObject);
	pDirectionalLight->SetAmbientColor(glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
	pDirectionalLight->SetDiffuseColor(glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
	pDirectionalLight->SetSpecularColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectionalLight->SetIntensity(1.0f);
	pDirectionalLightGameObject->GetTransform()->SetWorldPosition(glm::vec3(-1.0f, -1.0f, 1.0f));

	std::vector<PointLight*> pointLights;
	GameObject* pPointLightGameObject;
	PointLight* pPointLight;

	for (unsigned int z = 0; z < FLOOR_SIZE; z += 13)
	{
		for (unsigned int x = 0; x < FLOOR_SIZE; x += 13)
		{
			pPointLightGameObject = GameObject::Instantiate();
			//pPointLightGameObject->GetTransform()->SetLocalPosition(glm::vec3(-2.5f, 0.0f, -2.5f));
			pPointLightGameObject->GetTransform()->Translate(glm::vec3(-(FLOOR_SIZE * 0.5f - 10.0f) + x, 10.0f, -(FLOOR_SIZE * 0.5f - 10.0f) + z));

			pPointLight = PointLight::Instantiate(pPointLightGameObject);
			pPointLight->SetAmbientColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			glm::vec4 color = Random::NextColor();
			pPointLight->SetDiffuseColor(color);
			pPointLight->SetSpecularColor(color);
			pPointLight->SetIntensity(10.0f);
			pPointLight->SetConstantAttenuation(0.0f);
			pPointLight->SetLinearAttenuation(0.0f);
			pPointLight->SetQuadraticAttenuation(1.0f);

			pointLights.push_back(pPointLight);
		}
	}

	mpCheckersColorMapTexture = TextureUtils::LoadPNGAsTexture("textures/CheckersColorMap.png");
	mpSphereColorMapTexture = TextureUtils::LoadPNGAsTexture("textures/SteelColorMap.png");

#ifdef USE_PROGRAMMABLE_PIPELINE
	mpFloorMaterial = new Material(ShaderRegistry::Find("Diffuse"));
	mpFloorMaterial->SetTexture("colorMap", mpCheckersColorMapTexture);
#else
	mpFloorMaterial = new Material();
	mpFloorMaterial->SetTexture(mpCheckersColorMapTexture);
	mpFloorMaterial->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	mpFloorMaterial->SetShininess(5.0f);
#endif

	mpFloorMesh = StandardGeometries::CreateXZPlane((float)FLOOR_SIZE, (float)FLOOR_SIZE, 100, 100, glm::vec3(0, 0, 0));

	GameObject* pFloorGameObject = GameObject::Instantiate();
	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pFloorGameObject);
	pMeshRenderer->AddMesh(mpFloorMesh);
	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pFloorGameObject);
	pMeshFilter->SetMaterial(mpFloorMaterial);

#ifdef USE_PROGRAMMABLE_PIPELINE
	mpSphereMaterial = new Material(ShaderRegistry::Find("Diffuse"));
	mpSphereMaterial->SetTexture("colorMap", mpSphereColorMapTexture);
#else
	mpSphereMaterial = new Material();
	mpSphereMaterial->SetTexture(mSphereColorMapTextures[0]);
#endif

	mpSphereMesh = StandardGeometries::CreateSphere(SPHERE_RADIUS, NUMBER_OF_SPHERE_SLICES);

	std::vector<GameObject*> spheres;
	for (unsigned int z = 0; z < FLOOR_SIZE; z += 10)
	{
		for (unsigned int x = 0; x < FLOOR_SIZE; x += 10)
		{
			GameObject* pSphereGameObject = GameObject::Instantiate();
			pMeshRenderer = MeshRenderer::Instantiate(pSphereGameObject);
			pMeshRenderer->AddMesh(mpSphereMesh);
			pMeshFilter = MeshFilter::Instantiate(pSphereGameObject);
			pMeshFilter->SetMaterial(mpSphereMaterial);
			pSphereGameObject->GetTransform()->Translate(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));
			spheres.push_back(pSphereGameObject);
		}
	}

	GameObject* pGeneralBehavioursGameObject = GameObject::Instantiate();

	LightsAnimator* pLightsAnimator = LightsAnimator::Instantiate(pGeneralBehavioursGameObject);
	pLightsAnimator->SetLights(pointLights);

	SpheresAnimator* pSpheresAnimator = SpheresAnimator::Instantiate(pGeneralBehavioursGameObject);
	pSpheresAnimator->SetSpheres(spheres);
	pSpheresAnimator->SetRotationSpeed(50.0f);
	pSpheresAnimator->SetRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));

	KeyBindings* pKeyBindings = KeyBindings::Instantiate(pGeneralBehavioursGameObject);

	FirstPersonCameraController* pFirstPersonCameraController = FirstPersonCameraController::Instantiate(pGeneralBehavioursGameObject);
	pFirstPersonCameraController->SetWalkSpeed(WALK_SPEED);
	pFirstPersonCameraController->SetTurnSpeed(TURN_SPEED);
	pFirstPersonCameraController->SetFlying(true);
}

void DeferredShadingApplication::OnEnd()
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

	if (mpSphereColorMapTexture != 0)
	{
		delete mpSphereColorMapTexture;
		mpSphereColorMapTexture = 0;
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
