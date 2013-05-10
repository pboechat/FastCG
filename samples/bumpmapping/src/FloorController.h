#ifndef FLOORCONTROLLER_H_
#define FLOORCONTROLLER_H_

#include <Behaviour.h>
#include <Texture.h>

#include <glm/glm.hpp>

#include <vector>

class FloorController : public Behaviour
{
	DECLARE_TYPE;

public:
	FloorController() :
		mCurrentTextureIndex(0),
		mTiling(1, 1),
		mLastLeftButtonClickTime(0.0f),
		mLastMiddleButtonClickTime(0.0f)
	{
	}

	inline std::vector<TexturePtr> GetColorMapTextures() const
	{
		return mColorMapTextures;
	}

	inline void SetColorMapTextures(std::vector<TexturePtr> colorMapTextures)
	{
		mColorMapTextures = colorMapTextures;
	}

	inline std::vector<TexturePtr> GetBumpMapTextures() const
	{
		return mBumpMapTextures;
	}

	inline void SetBumpMapTextures(std::vector<TexturePtr> bumpMapTextures)
	{
		mBumpMapTextures = bumpMapTextures;
	}

	virtual void OnUpdate(float time, float deltaTime);

private:
	std::vector<TexturePtr> mColorMapTextures;
	std::vector<TexturePtr> mBumpMapTextures;
	unsigned int mCurrentTextureIndex;
	glm::vec2 mTiling;
	float mLastLeftButtonClickTime;
	float mLastMiddleButtonClickTime;

};

typedef Pointer<FloorController> FloorControllerPtr;

#endif