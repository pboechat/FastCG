#include "BumpMappingApplication.h"
#include "FloorController.h"
#include "SpheresController.h"
#include "LightAnimator.h"

#include <Material.h>
#include <PointLight.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <FirstPersonCameraController.h>
#include <Colors.h>
#include <TextureImporter.h>
#include <ModelImporter.h>
#include <OpenGLExceptions.h>

const unsigned int BumpMappingApplication::FLOOR_SIZE = 100;
const float BumpMappingApplication::SPHERE_RADIUS = (FLOOR_SIZE * 0.025f);
const unsigned int BumpMappingApplication::NUMBER_OF_SPHERE_SLICES = 30;
const float BumpMappingApplication::WALK_SPEED = 20.0f;
const float BumpMappingApplication::TURN_SPEED = 100.0f;

BumpMappingApplication::BumpMappingApplication() :
	Application("bumpmapping", 1024, 768, 60, false, "../../core/"),
	mpFloorMaterial(0),
	mpFloorMesh(0),
	mpSphereMaterial(0),
	mpSphereMesh(0)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void BumpMappingApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0.0f, FLOOR_SIZE * 0.5f, (float)FLOOR_SIZE));
	mpMainCamera->GetGameObject()->GetTransform()->RotateAroundLocal(-20.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	GameObject* pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(0.0f, FLOOR_SIZE * 0.25f, 0.0f));

	PointLight* pLight = PointLight::Instantiate(pLightGameObject);
	pLight->SetDiffuseColor(Colors::WHITE);
	pLight->SetSpecularColor(Colors::WHITE);
	pLight->SetIntensity(0.4f);

	LightAnimator* pLightAnimator = LightAnimator::Instantiate(pLightGameObject);
	pLightAnimator->SetSpeed(40.0f);
	pLightAnimator->SetAmplitude((float)FLOOR_SIZE);
	pLightAnimator->SetDirection(glm::vec3(0.0f, 0.0f, 1.0f));

	mFloorColorMapTextures.push_back(TextureImporter::Import("textures/FloorColorMap1.png"));
	mFloorBumpMapTextures.push_back(TextureImporter::Import("textures/FloorBumpMap1.png"));
	mFloorColorMapTextures.push_back(TextureImporter::Import("textures/FloorColorMap2.png"));
	mFloorBumpMapTextures.push_back(TextureImporter::Import("textures/FloorBumpMap2.png"));

	mSphereColorMapTextures.push_back(TextureImporter::Import("textures/SphereColorMap1.png"));
	mSphereBumpMapTextures.push_back(TextureImporter::Import("textures/SphereBumpMap1.png"));
	mSphereColorMapTextures.push_back(TextureImporter::Import("textures/SphereColorMap2.png"));
	mSphereBumpMapTextures.push_back(TextureImporter::Import("textures/SphereBumpMap2.png"));
	mSphereColorMapTextures.push_back(TextureImporter::Import("textures/SphereColorMap3.png"));
	mSphereBumpMapTextures.push_back(TextureImporter::Import("textures/SphereBumpMap3.png"));

#ifdef FIXED_FUNCTION_PIPELINE
	mpFloorMaterial = new Material();
	mpFloorMaterial->SetTexture(mFloorColorMapTextures[0]);
	mpFloorMaterial->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	mpFloorMaterial->SetShininess(5.0f);
#else
	Shader* pShader = ShaderRegistry::Find("BumpedSpecular");
	mpFloorMaterial = new Material(pShader);
	mpFloorMaterial->SetTexture("colorMap", mFloorColorMapTextures[0]);
	mpFloorMaterial->SetTexture("bumpMap", mFloorBumpMapTextures[0]);
	mpFloorMaterial->SetVec4("ambientColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mpFloorMaterial->SetVec4("diffuseColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mpFloorMaterial->SetVec4("specularColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	mpFloorMaterial->SetFloat("shininess", 5.0f);
#endif

	GameObject* pFloorGameObject = GameObject::Instantiate();

	mpFloorMesh = StandardGeometries::CreateXZPlane((float)FLOOR_SIZE, (float)FLOOR_SIZE, 1, 1, glm::vec3(0, 0, 0));

	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pFloorGameObject);
	pMeshRenderer->AddMesh(mpFloorMesh);

	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pFloorGameObject);
	pMeshFilter->SetMaterial(mpFloorMaterial);

	FloorController* pFloorController = FloorController::Instantiate(pFloorGameObject);
	pFloorController->SetColorMapTextures(mFloorColorMapTextures);
	pFloorController->SetBumpMapTextures(mFloorBumpMapTextures);

#ifdef FIXED_FUNCTION_PIPELINE
	mpSphereMaterial = new Material();
	mpSphereMaterial->SetTexture(mSphereColorMapTextures[0]);
	mpSphereMaterial->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	mpSphereMaterial->SetShininess(10.0f);
#else
	mpSphereMaterial = new Material(ShaderRegistry::Find("BumpedSpecular"));
	mpSphereMaterial->SetTexture("colorMap", mSphereColorMapTextures[0]);
	mpSphereMaterial->SetTexture("bumpMap", mSphereBumpMapTextures[0]);
	mpSphereMaterial->SetVec4("ambientColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mpSphereMaterial->SetVec4("diffuseColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mpSphereMaterial->SetVec4("specularColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	mpSphereMaterial->SetFloat("shininess", 10.0f);
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

			pSphereGameObject->GetTransform()->SetPosition(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));

			spheres.push_back(pSphereGameObject);
		}
	}

	GameObject* pSpheresControllerGameObject = GameObject::Instantiate();

	SpheresController* pSpheresController = SpheresController::Instantiate(pSpheresControllerGameObject);
	pSpheresController->SetSpheres(spheres);
	pSpheresController->SetRotationSpeed(50.0f);
	pSpheresController->SetRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));
	pSpheresController->SetColorMapTextures(mSphereColorMapTextures);
	pSpheresController->SetBumpMapTextures(mSphereBumpMapTextures);

	GameObject* pPlayerGameObject = GameObject::Instantiate();

	FirstPersonCameraController* pFirstPersonCameraController = FirstPersonCameraController::Instantiate(pPlayerGameObject);
	pFirstPersonCameraController->SetWalkSpeed(WALK_SPEED);
	pFirstPersonCameraController->SetTurnSpeed(TURN_SPEED);
	pFirstPersonCameraController->SetFlying(true);
}

void BumpMappingApplication::OnEnd()
{
	for (unsigned int i = 0; i < mFloorColorMapTextures.size(); i++)
	{
		delete mFloorColorMapTextures[i];
	}
	mFloorColorMapTextures.clear();

	for (unsigned int i = 0; i < mFloorBumpMapTextures.size(); i++)
	{
		delete mFloorBumpMapTextures[i];
	}
	mFloorBumpMapTextures.clear();

	for (unsigned int i = 0; i < mSphereColorMapTextures.size(); i++)
	{
		delete mSphereColorMapTextures[i];
	}
	mSphereColorMapTextures.clear();

	for (unsigned int i = 0; i < mSphereBumpMapTextures.size(); i++)
	{
		delete mSphereBumpMapTextures[i];
	}
	mSphereBumpMapTextures.clear();

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
		mpSphereMaterial = 0;
	}

	if (mpSphereMesh != 0)
	{
		delete mpSphereMesh;
		mpSphereMesh = 0;
	}
}
