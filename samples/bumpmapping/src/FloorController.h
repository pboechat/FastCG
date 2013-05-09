#ifndef FLOORCONTROLLER_H_
#define FLOORCONTROLLER_H_

#include <Updateable.h>
#include <TriangleMesh.h>
#include <Texture.h>
#include <Material.h>

#include <vector>

class FloorController : public Updateable
{
public:
	FloorController(TriangleMeshPtr floorPtr, const std::vector<TexturePtr>& rColorMapTexturesPtrs, const std::vector<TexturePtr>& rBumpMapTexturesPtrs);

	virtual void Update(float time, float deltaTime);

private:
	TriangleMeshPtr mFloorPtr;
	MaterialPtr mFloorMaterialPtr;
	std::vector<TexturePtr> mColorMapTexturesPtrs;
	std::vector<TexturePtr> mBumpMapTexturesPtrs;
	unsigned int mCurrentTextureIndex;
	glm::vec2 mTiling;
	float mLastLeftButtonClickTime;
	float mLastMiddleButtonClickTime;
};

#endif