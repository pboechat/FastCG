#include "BumpMappingApplication.h"
#include "FloorController.h"
#include "SpheresController.h"
#include "LightAnimator.h"

#include <Material.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <PNGLoader.h>
#include <FirstPersonCameraController.h>
#include <OpenGLExceptions.h>

#include <vld.h>

const unsigned int BumpMappingApplication::FLOOR_SIZE = 100;
const float BumpMappingApplication::SPHERE_RADIUS = (FLOOR_SIZE * 0.025f);
const unsigned int BumpMappingApplication::NUMBER_OF_SPHERE_SLICES = 10;
const float BumpMappingApplication::WALK_SPEED = 20.0f;
const float BumpMappingApplication::TURN_SPEED = 100.0f;

BumpMappingApplication::BumpMappingApplication() :
	Application("bumpmapping", 1024, 768)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mGlobalAmbientLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mShowFPS = true;
}

BumpMappingApplication::~BumpMappingApplication()
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

	delete mpFloorMaterial;
	delete mpFloorMesh;
	delete mpSphereMaterial;
	delete mpSphereMesh;
}

Texture* BumpMappingApplication::LoadPNGAsTexture(const std::string& rFileName)
{
	unsigned int width;
	unsigned int height;
	bool transparency;
	unsigned char* pData;
	PNGLoader::Load(rFileName, width, height, transparency, &pData);
	Texture* pTexture = new Texture(width, height, (transparency) ? TF_RGBA : TF_RGB, DT_UNSIGNED_CHAR, FM_BILINEAR, WM_REPEAT, pData);
	CHECK_FOR_OPENGL_ERRORS();
	return pTexture;
}

void BumpMappingApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, FLOOR_SIZE * 0.5f, (float)FLOOR_SIZE));
	mpMainCamera->GetGameObject()->GetTransform()->Rotate(-20.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	GameObject* pLightGameObject = GameObject::Instantiate();

	Light* pLight = Light::Instantiate(pLightGameObject);
	pLight->SetAmbientColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	pLight->SetDiffuseColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
	pLight->SetSpecularColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

	LightAnimator* pLightAnimator = LightAnimator::Instantiate(pLightGameObject);
	pLightAnimator->SetSpeed(40.0f);
	pLightAnimator->SetAmplitude((float)FLOOR_SIZE);
	pLightAnimator->SetDirection(glm::vec3(0.0f, 0.0f, 1.0f));

	pLightGameObject->GetTransform()->SetPosition(glm::vec3(0.0f, FLOOR_SIZE * 0.25f, 0.0f));

	mFloorColorMapTextures.push_back(LoadPNGAsTexture("textures/FloorColorMap1.png"));
	mFloorBumpMapTextures.push_back(LoadPNGAsTexture("textures/FloorBumpMap1.png"));
	mFloorColorMapTextures.push_back(LoadPNGAsTexture("textures/FloorColorMap2.png"));
	mFloorBumpMapTextures.push_back(LoadPNGAsTexture("textures/FloorBumpMap2.png"));

	mSphereColorMapTextures.push_back(LoadPNGAsTexture("textures/SphereColorMap1.png"));
	mSphereBumpMapTextures.push_back(LoadPNGAsTexture("textures/SphereColorMap1.png"));
	mSphereColorMapTextures.push_back(LoadPNGAsTexture("textures/SphereColorMap2.png"));
	mSphereBumpMapTextures.push_back(LoadPNGAsTexture("textures/SphereColorMap2.png"));

#ifdef USE_PROGRAMMABLE_PIPELINE
	Shader* pShader = ShaderRegistry::Find("BumpedSpecular");
	mpFloorMaterial = new Material(pShader);
	mpFloorMaterial->SetTexture("colorMap", mFloorColorMapTextures[0]);
	mpFloorMaterial->SetTexture("bumpMap", mFloorBumpMapTextures[0]);
	mpFloorMaterial->SetVec4("specularColor", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	mpFloorMaterial->SetFloat("shininess", 5.0f);
#else
	mpFloorMaterial = new Material();
	mpFloorMaterial->SetTexture(mFloorColorMapTextures[0]);
	mpFloorMaterial->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	mpFloorMaterial->SetShininess(5.0f);
#endif

	GameObject* pFloorGameObject = GameObject::Instantiate();

	mpFloorMesh = StandardGeometries::CreateXZPlane((float)FLOOR_SIZE, (float)FLOOR_SIZE, 100, 100, glm::vec3(0, 0, 0));

	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pFloorGameObject);
	pMeshRenderer->SetMesh(mpFloorMesh);

	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pFloorGameObject);
	pMeshFilter->SetMaterial(mpFloorMaterial);

	FloorController* pFloorController = FloorController::Instantiate(pFloorGameObject);
	pFloorController->SetColorMapTextures(mFloorColorMapTextures);
	pFloorController->SetBumpMapTextures(mFloorBumpMapTextures);

#ifdef USE_PROGRAMMABLE_PIPELINE
	mpSphereMaterial = new Material(ShaderRegistry::Find("BumpedSpecular"));
	mpSphereMaterial->SetTexture("colorMap", mSphereColorMapTextures[0]);
	mpSphereMaterial->SetTexture("bumpMap", mSphereBumpMapTextures[0]);
	mpSphereMaterial->SetVec4("specularColor", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	mpSphereMaterial->SetFloat("shininess", 10.0f);
#else
	mpSphereMaterial = new Material();
	mpSphereMaterial->SetTexture(mSphereColorMapTextures[0]);
	mpSphereMaterial->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	mpSphereMaterial->SetShininess(10.0f);
#endif

	mpSphereMesh = StandardGeometries::CreateSphere(SPHERE_RADIUS, NUMBER_OF_SPHERE_SLICES);

	std::vector<GameObject*> spheres;
	for (unsigned int z = 0; z < FLOOR_SIZE; z += 10)
	{
		for (unsigned int x = 0; x < FLOOR_SIZE; x += 10)
		{
			GameObject* pSphereGameObject = GameObject::Instantiate();

			pMeshRenderer = MeshRenderer::Instantiate(pSphereGameObject);
			pMeshRenderer->SetMesh(mpSphereMesh);

			pMeshFilter = MeshFilter::Instantiate(pSphereGameObject);
			pMeshFilter->SetMaterial(mpSphereMaterial);

			pSphereGameObject->GetTransform()->Translate(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));

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
}