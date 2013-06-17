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

const float BumpMappingApplication::WALK_SPEED = 20;
const float BumpMappingApplication::TURN_SPEED = 100;

BumpMappingApplication::BumpMappingApplication() :
	Application("bumpmapping", 1024, 768, 60, false, "../../core/"),
	mpFloorMaterial(0),
	mpFloorMesh(0),
	mpSphereMaterial(0),
	mpSphereMesh(0)
{
	mClearColor = glm::vec4(0, 0, 0, 1);
	mShowFPS = true;
	mShowRenderingStatistics = true;
}

void BumpMappingApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->SetPosition(glm::vec3(0, 5, 10));
	mpMainCamera->GetGameObject()->GetTransform()->RotateAroundLocal(-20, glm::vec3(1, 0, 0));

	GameObject* pLightGameObject = GameObject::Instantiate();
	pLightGameObject->GetTransform()->SetPosition(glm::vec3(0, 2.5f, 0));

	PointLight* pLight = PointLight::Instantiate(pLightGameObject);
	pLight->SetDiffuseColor(Colors::WHITE);
	pLight->SetSpecularColor(Colors::WHITE);
	pLight->SetQuadraticAttenuation(0.25f);
	pLight->SetIntensity(0.8f);

	LightAnimator* pLightAnimator = LightAnimator::Instantiate(pLightGameObject);
	pLightAnimator->SetSpeed(4);
	pLightAnimator->SetAmplitude(10);
	pLightAnimator->SetDirection(glm::vec3(0, 0, 1));

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
	mpFloorMaterial->SetSpecularColor(Colors::WHITE);
	mpFloorMaterial->SetShininess(5);
#else
	Shader* pShader = ShaderRegistry::Find("BumpedSpecular");
	mpFloorMaterial = new Material(pShader);
	mpFloorMaterial->SetTexture("colorMap", mFloorColorMapTextures[0]);
	mpFloorMaterial->SetTexture("bumpMap", mFloorBumpMapTextures[0]);
	mpFloorMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpFloorMaterial->SetVec4("specularColor", Colors::WHITE);
	mpFloorMaterial->SetFloat("shininess", 5);
#endif

	GameObject* pFloorGameObject = GameObject::Instantiate();

	mpFloorMesh = StandardGeometries::CreateXZPlane(10, 10, 1, 1, glm::vec3(0, 0, 0));

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
	mpSphereMaterial->SetSpecularColor(Colors::WHITE);
	mpSphereMaterial->SetShininess(10);
#else
	mpSphereMaterial = new Material(ShaderRegistry::Find("BumpedSpecular"));
	mpSphereMaterial->SetTexture("colorMap", mSphereColorMapTextures[0]);
	mpSphereMaterial->SetTexture("bumpMap", mSphereBumpMapTextures[0]);
	mpSphereMaterial->SetVec4("diffuseColor", Colors::WHITE);
	mpSphereMaterial->SetVec4("specularColor", Colors::WHITE);
	mpSphereMaterial->SetFloat("shininess", 10);
#endif

	mpSphereMesh = StandardGeometries::CreateSphere(0.25f, 30);

	std::vector<GameObject*> spheres;
	for (unsigned int z = 0; z < 10; z ++)
	{
		for (unsigned int x = 0; x < 10; x++)
		{
			GameObject* pSphereGameObject = GameObject::Instantiate();

			pMeshRenderer = MeshRenderer::Instantiate(pSphereGameObject);
			pMeshRenderer->AddMesh(mpSphereMesh);

			pMeshFilter = MeshFilter::Instantiate(pSphereGameObject);
			pMeshFilter->SetMaterial(mpSphereMaterial);

			pSphereGameObject->GetTransform()->SetPosition(glm::vec3(-4.5f + x, 0.25f, -4.5f + z));

			spheres.push_back(pSphereGameObject);
		}
	}

	GameObject* pSpheresControllerGameObject = GameObject::Instantiate();

	SpheresController* pSpheresController = SpheresController::Instantiate(pSpheresControllerGameObject);
	pSpheresController->SetSpheres(spheres);
	pSpheresController->SetRotationSpeed(50);
	pSpheresController->SetRotationAxis(glm::vec3(0, 1, 0));
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
