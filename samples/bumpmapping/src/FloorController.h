#ifndef FLOORCONTROLLER_H_
#define FLOORCONTROLLER_H_

#include <Behaviour.h>
#include <Texture.h>

#include <glm/glm.hpp>

#include <vector>

COMPONENT(FloorController, Behaviour)
public:
	virtual void OnInstantiate() 
	{
		mCurrentTextureIndex = 0;
		mTiling = glm::vec2(1, 1);
		mLastLeftButtonClickTime = 0.0f,
		mLastMiddleButtonClickTime = 0.0f;
	}

	inline void SetColorMapTextures(std::vector<Texture*> colorMapTextures)
	{
		mColorMapTextures = colorMapTextures;
	}

	inline void SetBumpMapTextures(std::vector<Texture*> bumpMapTextures)
	{
		mBumpMapTextures = bumpMapTextures;
	}

	virtual void OnUpdate(float time, float deltaTime);

private:
	std::vector<Texture*> mColorMapTextures;
	std::vector<Texture*> mBumpMapTextures;
	unsigned int mCurrentTextureIndex;
	glm::vec2 mTiling;
	float mLastLeftButtonClickTime;
	float mLastMiddleButtonClickTime;

};

#endif