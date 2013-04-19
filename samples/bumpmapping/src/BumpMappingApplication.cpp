#include "BumpMappingApplication.h"
#include <StandardGeometries.h>
#include <Material.h>
#include <OpenGLExceptions.h>
#include <PNGLoader.h>

#include <iostream>

BumpMappingApplication::BumpMappingApplication() :
	Application("bumpmapping", 800, 600)
{
	mClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mMainCamera.Translate(glm::vec3(0, 50.0f, 90.0f));
}

BumpMappingApplication::~BumpMappingApplication()
{
}

void BumpMappingApplication::OnStart()
{
	LightPtr lightPtr = new Light();
	lightPtr->SetDiffuseColor(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	lightPtr->Translate(glm::vec3(0.0f, 20.0f, 0.0f));
	AddLight(lightPtr);

	mBumpedDiffuseShaderPtr = new Shader("BumpedDiffuse");
	mBumpedDiffuseShaderPtr->Compile("shaders/BumpedDiffuse.vert", ST_VERTEX);
	mBumpedDiffuseShaderPtr->Compile("shaders/BumpedDiffuse.frag", ST_FRAGMENT);
	mBumpedDiffuseShaderPtr->Link();

	mGrassColorMapTexture = LoadPNGAsTexture("textures/GrassColorMap.png");
	CHECK_FOR_OPENGL_ERRORS();
	mGrassBumpMapTexture = LoadPNGAsTexture("textures/GrassBumpMap.png");
	CHECK_FOR_OPENGL_ERRORS();
	mTennisBallColorMapTexture = LoadPNGAsTexture("textures/TennisBallColorMap.png");
	CHECK_FOR_OPENGL_ERRORS();
	mTennisBallBumpMapTexture = LoadPNGAsTexture("textures/TennisBallBumpMap.png");
	CHECK_FOR_OPENGL_ERRORS();

	MaterialPtr spGroundMaterial = new Material(mBumpedDiffuseShaderPtr);
	spGroundMaterial->SetTexture("colorMap", mGrassColorMapTexture);
	//spGroundMaterial->SetTexture("bumpMap", mGrassBumpMapTexture);
	spGroundMaterial->SetVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	GeometryPtr spGround = StandardGeometries::CreateXZPlane(100, 100, 1, 1, glm::vec3(0, 0, 0), spGroundMaterial);
	AddGeometry(spGround);

	for (unsigned int z = 0; z < 100; z += 10)
	{
		for (unsigned int x = 0; x < 100; x += 10)
		{
			MaterialPtr spSphereMaterial = new Material(mBumpedDiffuseShaderPtr);
			spSphereMaterial->SetTexture("colorMap", mTennisBallColorMapTexture);
			//spSphereMaterial->SetTexture("bumpMap", mTennisBallBumpMapTexture);
			spSphereMaterial->SetVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			GeometryPtr spSphere = StandardGeometries::CreateSphere(2.5f, 30, 30, spSphereMaterial);
			spSphere->Translate(glm::vec3(-45.0f + x, 2.5f, -45.0f + z));
			AddGeometry(spSphere);
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