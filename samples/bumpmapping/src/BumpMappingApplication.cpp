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

const unsigned int BumpMappingApplication::FLOOR_SIZE = 100;
const float BumpMappingApplication::SPHERE_RADIUS = (FLOOR_SIZE * 0.025f);
const unsigned int BumpMappingApplication::NUMBER_OF_SPHERE_SLICES = 30;
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
}

TexturePtr BumpMappingApplication::LoadPNGAsTexture(const std::string& rFileName)
{
	unsigned int width;
	unsigned int height;
	bool transparency;
	unsigned char* pData;
	PNGLoader::Load(rFileName, width, height, transparency, &pData);
	TexturePtr texturePtr = new Texture(width, height, (transparency) ? TF_RGBA : TF_RGB, DT_UNSIGNED_CHAR, FM_BILINEAR, WM_REPEAT, pData);
	CHECK_FOR_OPENGL_ERRORS();
	return texturePtr;
}

void BumpMappingApplication::OnStart()
{
	mpMainCamera->GetGameObject()->GetTransform()->Translate(glm::vec3(0.0f, FLOOR_SIZE * 0.5f, (float)FLOOR_SIZE));
	mpMainCamera->GetGameObject()->GetTransform()->Rotate(-20.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	GameObjectPtr lightGameObjectPtr = new GameObject();

	LightPtr lightPtr = new Light();
	lightPtr->SetAmbientColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	lightPtr->SetDiffuseColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
	lightPtr->SetSpecularColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
	
	lightGameObjectPtr->AddComponent(lightPtr);

	LightAnimatorPtr lightAnimatorPtr = new LightAnimator();
	lightAnimatorPtr->SetSpeed(40.0f);
	lightAnimatorPtr->SetAmplitude((float)FLOOR_SIZE);
	lightAnimatorPtr->SetDirection(glm::vec3(0.0f, 0.0f, 1.0f));

	lightGameObjectPtr->AddComponent(lightAnimatorPtr);

	lightGameObjectPtr->GetTransform()->SetPosition(glm::vec3(0.0f, FLOOR_SIZE * 0.25f, 0.0f));

	std::vector<TexturePtr> floorColorMapTextures;
	std::vector<TexturePtr> floorBumpMapTextures;
	floorColorMapTextures.push_back(LoadPNGAsTexture("textures/FloorColorMap1.png"));
	floorBumpMapTextures.push_back(LoadPNGAsTexture("textures/FloorBumpMap1.png"));
	floorColorMapTextures.push_back(LoadPNGAsTexture("textures/FloorColorMap2.png"));
	floorBumpMapTextures.push_back(LoadPNGAsTexture("textures/FloorBumpMap2.png"));

	std::vector<TexturePtr> sphereColorMapTextures;
	std::vector<TexturePtr> sphereBumpMapTextures;
	sphereColorMapTextures.push_back(LoadPNGAsTexture("textures/SphereColorMap1.png"));
	sphereBumpMapTextures.push_back(LoadPNGAsTexture("textures/SphereColorMap1.png"));
	sphereColorMapTextures.push_back(LoadPNGAsTexture("textures/SphereColorMap2.png"));
	sphereBumpMapTextures.push_back(LoadPNGAsTexture("textures/SphereColorMap2.png"));

	MaterialPtr floorMaterialPtr;
#ifdef USE_PROGRAMMABLE_PIPELINE
	floorMaterialPtr = new Material(ShaderRegistry::Find("BumpedSpecular"));
	floorMaterialPtr->SetTexture("colorMap", floorColorMapTextures[0]);
	floorMaterialPtr->SetTexture("bumpMap", floorBumpMapTextures[0]);
	floorMaterialPtr->SetVec4("specularColor", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	floorMaterialPtr->SetFloat("shininess", 5.0f);
#else
	floorMaterialPtr = new Material();
	floorMaterialPtr->SetTexture(floorColorMapTextures[0]);
	floorMaterialPtr->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	floorMaterialPtr->SetShininess(5.0f);
#endif

	GameObjectPtr floorGameObjectPtr = new GameObject();

	MeshRendererPtr meshRendererPtr = new MeshRenderer();
	MeshPtr floorMeshPtr = StandardGeometries::CreateXZPlane((float)FLOOR_SIZE, (float)FLOOR_SIZE, 100, 100, glm::vec3(0, 0, 0));
#ifdef USE_PROGRAMMABLE_PIPELINE
	floorMeshPtr->CalculateTangents();
#endif
	meshRendererPtr->SetMesh(floorMeshPtr);

	floorGameObjectPtr->AddComponent(meshRendererPtr);

	MeshFilterPtr meshFilterPtr = new MeshFilter();
	meshFilterPtr->SetMaterial(floorMaterialPtr);

	floorGameObjectPtr->AddComponent(meshFilterPtr);

	FloorControllerPtr floorControllerPtr = new FloorController();
	floorControllerPtr->SetColorMapTextures(floorColorMapTextures);
	floorControllerPtr->SetBumpMapTextures(floorBumpMapTextures);

	floorGameObjectPtr->AddComponent(floorControllerPtr);

	MaterialPtr sphereMaterialPtr;
#ifdef USE_PROGRAMMABLE_PIPELINE
	sphereMaterialPtr = new Material(ShaderRegistry::Find("BumpedSpecular"));
	sphereMaterialPtr->SetTexture("colorMap", sphereColorMapTextures[0]);
	sphereMaterialPtr->SetTexture("bumpMap", sphereBumpMapTextures[0]);
	sphereMaterialPtr->SetVec4("specularColor", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	sphereMaterialPtr->SetFloat("shininess", 10.0f);
#else
	sphereMaterialPtr = new Material();
	sphereMaterialPtr->SetTexture(sphereColorMapTextures[0]);
	sphereMaterialPtr->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	sphereMaterialPtr->SetShininess(10.0f);
#endif

	MeshPtr sphereMeshPtr = StandardGeometries::CreateSphere(SPHERE_RADIUS, NUMBER_OF_SPHERE_SLICES);
#ifdef USE_PROGRAMMABLE_PIPELINE
	sphereMeshPtr->CalculateTangents();
#endif

	std::vector<GameObjectPtr> spheres;
	for (unsigned int z = 0; z < FLOOR_SIZE; z += 10)
	{
		for (unsigned int x = 0; x < FLOOR_SIZE; x += 10)
		{
			GameObjectPtr sphereGameObjectPtr = new GameObject();

			meshRendererPtr = new MeshRenderer();
			meshRendererPtr->SetMesh(sphereMeshPtr);

			sphereGameObjectPtr->AddComponent(meshRendererPtr);

			meshFilterPtr = new MeshFilter();
			meshFilterPtr->SetMaterial(sphereMaterialPtr);

			sphereGameObjectPtr->AddComponent(meshFilterPtr);

			sphereGameObjectPtr->GetTransform()->Translate(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));

			spheres.push_back(sphereGameObjectPtr);
		}
	}

	GameObjectPtr spheresGameObjectPtr = new GameObject();

	SpheresControllerPtr spheresControllerPtr = new SpheresController();
	spheresControllerPtr->SetSpheres(spheres);
	spheresControllerPtr->SetRotationSpeed(50.0f);
	spheresControllerPtr->SetRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));
	spheresControllerPtr->SetColorMapTextures(sphereColorMapTextures);
	spheresControllerPtr->SetBumpMapTextures(sphereBumpMapTextures);

	spheresGameObjectPtr->AddComponent(spheresControllerPtr);

	GameObjectPtr playerGameObjectPtr = new GameObject();

	FirstPersonCameraControllerPtr firstPersonCameraControllerPtr = new FirstPersonCameraController();
	firstPersonCameraControllerPtr->SetWalkSpeed(WALK_SPEED);
	firstPersonCameraControllerPtr->SetTurnSpeed(TURN_SPEED);

	playerGameObjectPtr->AddComponent(firstPersonCameraControllerPtr);
}