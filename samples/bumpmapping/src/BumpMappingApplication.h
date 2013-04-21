#ifndef BUMPMAPPINGAPPLICATION_H_
#define BUMPMAPPINGAPPLICATION_H_

#include <Application.h>
#include <Light.h>
#include <Material.h>
#include <Texture.h>

#include <string>
#include <vector>

class BumpMappingApplication: public Application
{
public:
	BumpMappingApplication();
	virtual ~BumpMappingApplication();

protected:
	virtual void OnStart();
	virtual void BeforeDisplay();
	virtual void OnMouseButton(int button, int state, int x, int y);
	virtual void OnMouseWheel(int button, int direction, int x, int y);

private:
	static const unsigned int FIELD_SIZE;
	static const unsigned int NUM_SPHERE_SEGMENTS;

	LightPtr mLightPtr;
	glm::vec3 mLightPosition;
	glm::vec3 mLightDirection;
	std::vector<TexturePtr> mFloorColorMapTexturePtrs;
	std::vector<TexturePtr> mFloorBumpMapTexturePtrs;
	std::vector<TexturePtr> mSphereColorMapTexturePtrs;
	std::vector<TexturePtr> mSphereBumpMapTexturePtrs;
	std::vector<GeometryPtr> mSpherePtrs;
	MaterialPtr mFloorMaterialPtr;
	unsigned int mCurrentFloorTextureIndex;
	unsigned int mCurrentSphereTextureIndex;
	glm::vec2 mFloorTextureTiling;

	TexturePtr LoadPNGAsTexture(const std::string& rFileName);

};

#endif
