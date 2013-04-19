#include "BumpMappingApplication.h"
#include <StandardGeometries.h>
#include <Material.h>
#include <OpenGLExceptions.h>
#include <PNGLoader.h>

#include <iostream>

const unsigned int BumpMappingApplication::FIELD_SIZE = 100;
const unsigned int BumpMappingApplication::NUM_SPHERE_SEGMENTS = 30;

BumpMappingApplication::BumpMappingApplication() :
	Application("bumpmapping", 800, 600),
	mCurrentFloorTextureIndex(0),
	mCurrentSphereTextureIndex(0),
	mFloorTextureTiling(1.0f, 1.0f)
{
	mClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	mGlobalAmbientLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mMainCamera.Translate(glm::vec3(0, FIELD_SIZE * 0.5f, (float)FIELD_SIZE));
	mLightPosition = glm::vec3(0.0f, FIELD_SIZE * 0.25f, 0.0f);
	mLightDirection = glm::vec3(0.0f, 0.0f, -1.0f);
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

	PNGLoader::Load(rFileName, &width, &height, &transparency, &pData);

	TexturePtr texturePtr = new Texture(width, height, (transparency) ? TF_RGBA : TF_RGB, DT_UNSIGNED_CHAR, FM_BILINEAR, WM_REPEAT, pData);
	CHECK_FOR_OPENGL_ERRORS();

	return texturePtr;
}

void BumpMappingApplication::OnStart()
{
	mLightPtr = new Light();
	mLightPtr->SetAmbientColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	mLightPtr->SetDiffuseColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
	mLightPtr->SetSpecularColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
	mLightPtr->SetPosition(mLightPosition);
	AddLight(mLightPtr);

#ifdef USE_OPENGL4
	mBumpedDiffuseShaderPtr = new Shader("BumpedDiffuse");
	mBumpedDiffuseShaderPtr->Compile("shaders/BumpedDiffuse.vert", ST_VERTEX);
	mBumpedDiffuseShaderPtr->Compile("shaders/BumpedDiffuse.frag", ST_FRAGMENT);
	mBumpedDiffuseShaderPtr->Link();

	mBumpedSpecularShaderPtr = new Shader("BumpedSpecular");
	mBumpedSpecularShaderPtr->Compile("shaders/BumpedSpecular.vert", ST_VERTEX);
	mBumpedSpecularShaderPtr->Compile("shaders/BumpedSpecular.frag", ST_FRAGMENT);
	mBumpedSpecularShaderPtr->Link();
#endif

	mFloorColorMapTexturePtrs.push_back(LoadPNGAsTexture("textures/FloorColorMap1.png"));
	mFloorBumpMapTexturePtrs.push_back(LoadPNGAsTexture("textures/FloorBumpMap1.png"));

	mFloorColorMapTexturePtrs.push_back(LoadPNGAsTexture("textures/FloorColorMap2.png"));
	mFloorBumpMapTexturePtrs.push_back(LoadPNGAsTexture("textures/FloorBumpMap2.png"));

	mSphereColorMapTexturePtrs.push_back(LoadPNGAsTexture("textures/SphereColorMap1.png"));
	mSphereBumpMapTexturePtrs.push_back(LoadPNGAsTexture("textures/SphereColorMap1.png"));

#ifdef USE_OPENGL4
	mFloorMaterialPtr = new Material(mBumpedSpecularShaderPtr);
	mFloorMaterialPtr->SetTexture("colorMap", mFloorColorMapTexturePtrs[mCurrentFloorTextureIndex]);
	mFloorMaterialPtr->SetTexture("bumpMap", mFloorBumpMapTexturePtrs[mCurrentFloorTextureIndex]);
	mFloorMaterialPtr->SetVec4("specularColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	mFloorMaterialPtr->SetFloat("shininess", 3.0f);
	mFloorMaterialPtr->SetTextureTiling("colorMap", mFloorTextureTiling);
	mFloorMaterialPtr->SetTextureTiling("bumpMap", mFloorTextureTiling);
#else
	mFloorMaterialPtr = new Material();
	mFloorMaterialPtr->SetTexture(mFloorColorMapTexturePtrs[mCurrentFloorTextureIndex]);
#endif

	GeometryPtr floorPtr = StandardGeometries::CreateXZPlane((float)FIELD_SIZE, (float)FIELD_SIZE, 100, 100, glm::vec3(0, 0, 0), mFloorMaterialPtr);
#ifdef USE_OPENGL4
	floorPtr->CalculateTangents();
#endif
	AddGeometry(floorPtr);

	for (unsigned int z = 0; z < FIELD_SIZE; z += 10)
	{
		for (unsigned int x = 0; x < FIELD_SIZE; x += 10)
		{
			MaterialPtr sphereMaterialPtr;

#ifdef USE_OPENGL4
			sphereMaterialPtr = new Material(mBumpedDiffuseShaderPtr);
			sphereMaterialPtr->SetTexture("colorMap", mSphereColorMapTexturePtrs[mCurrentSphereTextureIndex]);
			sphereMaterialPtr->SetTexture("bumpMap", mSphereBumpMapTexturePtrs[mCurrentSphereTextureIndex]);
#else
			sphereMaterialPtr = new Material();
			sphereMaterialPtr->SetTexture(mSphereColorMapTexturePtrs[mCurrentSphereTextureIndex]);
#endif

			float sphereRadius = (FIELD_SIZE * 0.025f);

			GeometryPtr spherePtr = StandardGeometries::CreateSphere(sphereRadius, NUM_SPHERE_SEGMENTS, NUM_SPHERE_SEGMENTS, sphereMaterialPtr);
#ifdef USE_OPENGL4
			spherePtr->CalculateTangents();
#endif
			spherePtr->Translate(glm::vec3(-(FIELD_SIZE * 0.5f - (2 * sphereRadius)) + x, sphereRadius, -(FIELD_SIZE * 0.5f - (2 * sphereRadius)) + z));
			mSpherePtrs.push_back(spherePtr);
			AddGeometry(spherePtr);
		}
	}
}

void BumpMappingApplication::BeforeDisplay()
{
#ifdef USE_OPENGL4
	mFloorMaterialPtr->SetTexture("colorMap", mFloorColorMapTexturePtrs[mCurrentFloorTextureIndex]);
	mFloorMaterialPtr->SetTexture("bumpMap", mFloorBumpMapTexturePtrs[mCurrentFloorTextureIndex]);
	mFloorMaterialPtr->SetTextureTiling("colorMap", mFloorTextureTiling);
	mFloorMaterialPtr->SetTextureTiling("bumpMap", mFloorTextureTiling);
#else
	mFloorMaterialPtr->SetTexture(mFloorColorMapTexturePtrs[mCurrentFloorTextureIndex]);
#endif

	for (unsigned int i = 0; i < mSpherePtrs.size(); i++)
	{
		GeometryPtr spherePtr = mSpherePtrs[i];
#ifdef USE_OPENGL4
		spherePtr->GetMaterial()->SetTexture("colorMap", mSphereColorMapTexturePtrs[mCurrentSphereTextureIndex]);
		spherePtr->GetMaterial()->SetTexture("bumpMap", mSphereBumpMapTexturePtrs[mCurrentSphereTextureIndex]);
#else
		spherePtr->GetMaterial()->SetTexture(mSphereColorMapTexturePtrs[mCurrentSphereTextureIndex]);
#endif
		spherePtr->Rotate(1.0f, glm::vec3(0, 1, 0));
	}

	mLightPosition += mLightDirection;

	if (mLightPosition.z <= -(FIELD_SIZE * 1.5f) || mLightPosition.z >= (FIELD_SIZE * 0.5f))
	{
		mLightDirection *= -1.0f;
	}

	mLightPtr->SetPosition(mLightPosition);
}

void BumpMappingApplication::OnMouseButton(int button, int state, int x, int y)
{
	if (state == 0)
	{
		if (button == 0)
		{
			mCurrentFloorTextureIndex = (++mCurrentFloorTextureIndex % mFloorColorMapTexturePtrs.size());
		}
		else if (button == 2)
		{
			mCurrentSphereTextureIndex = (++mCurrentSphereTextureIndex % mSphereColorMapTexturePtrs.size());
		}
	}
}

void BumpMappingApplication::OnMouseWheel(int button, int direction, int x, int y)
{
	if (direction > 0)
	{
		mFloorTextureTiling += glm::vec2(0.1f, 0.1f);
	}
	else
	{
		mFloorTextureTiling -= glm::vec2(0.1f, 0.1f);
	}
}
