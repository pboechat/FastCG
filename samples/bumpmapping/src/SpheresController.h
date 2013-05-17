#ifndef SPHERESCONTROLLER_H_
#define SPHERESCONTROLLER_H_

#include <Behaviour.h>
#include <Texture.h>

#include <glm/glm.hpp>

#include <vector>

COMPONENT(SpheresAnimator, Behaviour)
public:
	virtual void OnInstantiate()
	{
		mRotationSpeed = 0.0f;
		mRotationAxis = glm::vec3(0.0f, 0.0f, 0.0f);
		mLastLeftButtonClickTime = 0.0f;
		mLastKeyPressTime = 0.0f;
		mLastMiddleButtonClickTime = 0.0f;
		mCurrentTextureIndex = 0;
	}

	inline void SetSpheres(std::vector<GameObject*> spheres)
	{
		mSpheres = spheres;
	}

	inline void SetRotationSpeed(float rotationSpeed)
	{
		mRotationSpeed = rotationSpeed;
	}

	inline void SetRotationAxis(const glm::vec3& rotationAxis)
	{
		mRotationAxis = rotationAxis;
	}

	inline void SetColorMapTextures(const std::vector<Texture*>& rColorMapTextures)
	{
		mColorMapTextures = rColorMapTextures;
	}

	inline void SetBumpMapTextures(const std::vector<Texture*>& rBumpMapTextures)
	{
		mBumpMapTextures = rBumpMapTextures;
	}

	virtual void OnUpdate(float time, float deltaTime);

private:
	std::vector<GameObject*> mSpheres;
	float mRotationSpeed;
	glm::vec3 mRotationAxis;
	std::vector<Texture*> mColorMapTextures;
	std::vector<Texture*> mBumpMapTextures;
	unsigned int mCurrentTextureIndex;
	float mLastLeftButtonClickTime;
	float mLastKeyPressTime;
	float mLastMiddleButtonClickTime;

};

#endif