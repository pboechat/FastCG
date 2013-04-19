#include "BumpMappingApplication.h"
#include <StandardGeometries.h>
#include <Material.h>
#include <OpenGLExceptions.h>
#include <PNGLoader.h>

#include <iostream>

const unsigned int BumpMappingApplication::FIELD_SIZE = 100;
const unsigned int BumpMappingApplication::NUM_SPHERE_SEGMENTS = 30;

BumpMappingApplication::BumpMappingApplication() :
	Application("bumpmapping", 800, 600)
{
	mClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mGlobalAmbientLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mMainCamera.Translate(glm::vec3(0, FIELD_SIZE * 0.5f, (float)FIELD_SIZE));
}

BumpMappingApplication::~BumpMappingApplication()
{
}

void BumpMappingApplication::OnStart()
{
	LightPtr lightPtr = new Light();
	lightPtr->SetAmbientColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	lightPtr->SetDiffuseColor(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	lightPtr->SetSpecularColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	lightPtr->Translate(glm::vec3(0.0f, FIELD_SIZE * 0.25f, 0.0f));
	AddLight(lightPtr);

#ifdef USE_OPENGL4
	mBumpedDiffuseShaderPtr = new Shader("BumpedDiffuse");
	mBumpedDiffuseShaderPtr->Compile("shaders/BumpedDiffuse.vert", ST_VERTEX);
	mBumpedDiffuseShaderPtr->Compile("shaders/BumpedDiffuse.frag", ST_FRAGMENT);
	mBumpedDiffuseShaderPtr->Link();
#endif

	mGrassColorMapTexturePtr = LoadPNGAsTexture("textures/GrassColorMap.png");
	CHECK_FOR_OPENGL_ERRORS();
	mGrassBumpMapTexturePtr = LoadPNGAsTexture("textures/GrassBumpMap.png");
	CHECK_FOR_OPENGL_ERRORS();
	mTennisBallColorMapTexturePtr = LoadPNGAsTexture("textures/TennisBallColorMap.png");
	CHECK_FOR_OPENGL_ERRORS();
	mTennisBallBumpMapTexturePtr = LoadPNGAsTexture("textures/TennisBallBumpMap.png");
	CHECK_FOR_OPENGL_ERRORS();

	MaterialPtr groundMaterialPtr;

#ifdef USE_OPENGL4
	groundMaterialPtr = new Material(mBumpedDiffuseShaderPtr);
	groundMaterialPtr->SetTexture("colorMap", mGrassColorMapTexturePtr);
	groundMaterialPtr->SetTexture("bumpMap", mGrassBumpMapTexturePtr);
	groundMaterialPtr->SetVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
#else
	groundMaterialPtr = new Material();
	groundMaterialPtr->SetTexture(mGrassColorMapTexturePtr);
#endif

	GeometryPtr groundPtr = StandardGeometries::CreateXZPlane((float)FIELD_SIZE, (float)FIELD_SIZE, 1, 1, glm::vec3(0, 0, 0), groundMaterialPtr);
#ifdef USE_OPENGL4
	groundPtr->CalculateTangents();
#endif
	AddGeometry(groundPtr);

	for (unsigned int z = 0; z < FIELD_SIZE; z += 10)
	{
		for (unsigned int x = 0; x < FIELD_SIZE; x += 10)
		{
			MaterialPtr sphereMaterialPtr;

#ifdef USE_OPENGL4
			sphereMaterialPtr = new Material(mBumpedDiffuseShaderPtr);
			sphereMaterialPtr->SetTexture("colorMap", mTennisBallColorMapTexturePtr);
			sphereMaterialPtr->SetTexture("bumpMap", mTennisBallBumpMapTexturePtr);
			sphereMaterialPtr->SetVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
#else
			sphereMaterialPtr = new Material();
			sphereMaterialPtr->SetTexture(mTennisBallColorMapTexturePtr);
#endif

			float sphereRadius = (FIELD_SIZE * 0.025f);

			GeometryPtr spherePtr = StandardGeometries::CreateSphere(sphereRadius, NUM_SPHERE_SEGMENTS, NUM_SPHERE_SEGMENTS, sphereMaterialPtr);
#ifdef USE_OPENGL4
			spherePtr->CalculateTangents();
#endif
			spherePtr->Translate(glm::vec3(-(FIELD_SIZE * 0.5f - (2 * sphereRadius)) + x, sphereRadius, -(FIELD_SIZE * 0.5f - (2 * sphereRadius)) + z));
			AddGeometry(spherePtr);
		}
	}
}

TexturePtr BumpMappingApplication::LoadPNGAsTexture(const std::string& rFileName)
{
	unsigned int width;
	unsigned int height;
	bool transparency;
	unsigned char* pData;

	PNGLoader::Load(rFileName, &width, &height, &transparency, &pData);

	return new Texture(width, height, (transparency) ? TF_RGBA : TF_RGB, DT_UNSIGNED_CHAR, FM_BILINEAR, WM_CLAMP, pData);
}