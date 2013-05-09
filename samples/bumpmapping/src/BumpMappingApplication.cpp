#include "BumpMappingApplication.h"
#include "FloorController.h"
#include "SpheresController.h"
#include "LightAnimator.h"

#include <Light.h>
#include <Material.h>
#include <Texture.h>
#include <OpenGLExceptions.h>
#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <KeyCode.h>
#include <MouseButton.h>
#include <PNGLoader.h>
#include <FirstPersonCameraController.h>

const unsigned int BumpMappingApplication::FLOOR_SIZE = 100;
const float BumpMappingApplication::SPHERE_RADIUS = (FLOOR_SIZE * 0.025f);
const unsigned int BumpMappingApplication::NUMBER_OF_SPHERE_SLICES = 30;
const float BumpMappingApplication::WALK_SPEED = 20.0f;
const float BumpMappingApplication::TURN_SPEED = 60.0f;

BumpMappingApplication::BumpMappingApplication() :
	Application("bumpmapping", 800, 600)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mGlobalAmbientLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	mMainCameraPtr->Translate(glm::vec3(0.0f, FLOOR_SIZE * 0.5f, (float)FLOOR_SIZE));
	mMainCameraPtr->Rotate(-20.0f, glm::vec3(1.0f, 0.0f, 0.0f));

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
	LightPtr lightPtr = new Light();
	lightPtr->SetAmbientColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	lightPtr->SetDiffuseColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
	lightPtr->SetSpecularColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
	lightPtr->SetPosition(glm::vec3(0.0f, FLOOR_SIZE * 0.25f, 0.0f));
	AddLight(lightPtr);
	AddUpdateable(new LightAnimator(lightPtr, 40.0f, (float)FLOOR_SIZE, glm::vec3(0.0f, 0.0f, 1.0f)));

	std::vector<TexturePtr> floorColorMapTexturesPtrs;
	std::vector<TexturePtr> floorBumpMapTexturesPtrs;
	floorColorMapTexturesPtrs.push_back(LoadPNGAsTexture("textures/FloorColorMap1.png"));
	floorBumpMapTexturesPtrs.push_back(LoadPNGAsTexture("textures/FloorBumpMap1.png"));
	floorColorMapTexturesPtrs.push_back(LoadPNGAsTexture("textures/FloorColorMap2.png"));
	floorBumpMapTexturesPtrs.push_back(LoadPNGAsTexture("textures/FloorBumpMap2.png"));

	std::vector<TexturePtr> sphereColorMapTexturesPtrs;
	std::vector<TexturePtr> sphereBumpMapTexturesPtrs;
	sphereColorMapTexturesPtrs.push_back(LoadPNGAsTexture("textures/SphereColorMap1.png"));
	sphereBumpMapTexturesPtrs.push_back(LoadPNGAsTexture("textures/SphereColorMap1.png"));
	sphereColorMapTexturesPtrs.push_back(LoadPNGAsTexture("textures/SphereColorMap2.png"));
	sphereBumpMapTexturesPtrs.push_back(LoadPNGAsTexture("textures/SphereColorMap2.png"));

	MaterialPtr floorMaterialPtr;
#ifdef USE_PROGRAMMABLE_PIPELINE
	floorMaterialPtr = new Material(ShaderRegistry::Find("BumpedSpecular"));
	floorMaterialPtr->SetTexture("colorMap", floorColorMapTexturesPtrs[0]);
	floorMaterialPtr->SetTexture("bumpMap", floorBumpMapTexturesPtrs[0]);
	floorMaterialPtr->SetVec4("specularColor", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	floorMaterialPtr->SetFloat("shininess", 5.0f);
#else
	floorMaterialPtr = new Material();
	floorMaterialPtr->SetTexture(floorColorMapTexturesPtrs[0]);
	floorMaterialPtr->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	floorMaterialPtr->SetShininess(5.0f);
#endif
	TriangleMeshPtr floorPtr = StandardGeometries::CreateXZPlane((float)FLOOR_SIZE, (float)FLOOR_SIZE, 100, 100, glm::vec3(0, 0, 0), floorMaterialPtr);
#ifdef USE_PROGRAMMABLE_PIPELINE
	floorPtr->CalculateTangents();
#endif
	AddDrawable(floorPtr);
	AddUpdateable(new FloorController(floorPtr, floorColorMapTexturesPtrs, floorBumpMapTexturesPtrs));

	std::vector<TriangleMeshPtr> spheres;
	for (unsigned int z = 0; z < FLOOR_SIZE; z += 10)
	{
		for (unsigned int x = 0; x < FLOOR_SIZE; x += 10)
		{
			MaterialPtr sphereMaterialPtr;
#ifdef USE_PROGRAMMABLE_PIPELINE
			sphereMaterialPtr = new Material(ShaderRegistry::Find("BumpedSpecular"));
			sphereMaterialPtr->SetTexture("colorMap", sphereColorMapTexturesPtrs[0]);
			sphereMaterialPtr->SetTexture("bumpMap", sphereBumpMapTexturesPtrs[0]);
			sphereMaterialPtr->SetVec4("specularColor", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
			sphereMaterialPtr->SetFloat("shininess", 10.0f);
#else
			sphereMaterialPtr = new Material();
			sphereMaterialPtr->SetTexture(sphereColorMapTexturesPtrs[0]);
			sphereMaterialPtr->SetSpecularColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
			sphereMaterialPtr->SetShininess(10.0f);
#endif
			TriangleMeshPtr spherePtr = StandardGeometries::CreateSphere(SPHERE_RADIUS, NUMBER_OF_SPHERE_SLICES, sphereMaterialPtr);
#ifdef USE_PROGRAMMABLE_PIPELINE
			spherePtr->CalculateTangents();
#endif
			spherePtr->Translate(glm::vec3(-(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + x, SPHERE_RADIUS, -(FLOOR_SIZE * 0.5f - (2 * SPHERE_RADIUS)) + z));
			AddDrawable(spherePtr);
			spheres.push_back(spherePtr);
		}
	}
	AddUpdateable(new SpheresController(spheres, 50.0f, glm::vec3(0.0f, 1.0f, 0.0f), sphereColorMapTexturesPtrs, sphereBumpMapTexturesPtrs));

	AddUpdateable(new FirstPersonCameraController(WALK_SPEED, TURN_SPEED));
}