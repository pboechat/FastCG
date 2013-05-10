#ifndef SPHERESCONTROLLER_H_
#define SPHERESCONTROLLER_H_

#include <Behaviour.h>
#include <Texture.h>

#include <glm/glm.hpp>

#include <vector>

class SpheresController : public Behaviour
{
public:
	SpheresController() :
		mRotationSpeed(0),
		mRotationAxis(glm::vec3(0.0f, 0.0f, 0.0f)),
		mLastLeftButtonClickTime(0.0f),
		mLastKeyPressTime(0.0f),
		mLastMiddleButtonClickTime(0.0f),
		mCurrentTextureIndex(0)
	{
	}

	inline std::vector<GameObjectPtr> GetSpheres() const
	{
		return mSpheres;
	}

	inline void SetSpheres(std::vector<GameObjectPtr> spheres)
	{
		mSpheres = spheres;
	}

	inline float GetRotationSpeed() const
	{
		return mRotationSpeed;
	}
	inline void SetRotationSpeed(float rotationSpeed)
	{
		mRotationSpeed = rotationSpeed;
	}

	inline glm::vec3 GetRotationAxis() const
	{
		return mRotationAxis;
	}

	inline void SetRotationAxis(const glm::vec3& rotationAxis)
	{
		mRotationAxis = rotationAxis;
	}

	inline std::vector<TexturePtr> GetColorMapTextures() const
	{
		return mColorMapTextures;
	}

	inline void SetColorMapTextures(const std::vector<TexturePtr>& rColorMapTextures)
	{
		mColorMapTextures = rColorMapTextures;
	}

	inline std::vector<TexturePtr> GetBumpMapTextures() const
	{
		return mBumpMapTextures;
	}

	inline void SetBumpMapTextures(const std::vector<TexturePtr>& rBumpMapTextures)
	{
		mBumpMapTextures = rBumpMapTextures;
	}

	virtual void OnUpdate(float time, float deltaTime);

private:
	std::vector<GameObjectPtr> mSpheres;
	float mRotationSpeed;
	glm::vec3 mRotationAxis;
	std::vector<TexturePtr> mColorMapTextures;
	std::vector<TexturePtr> mBumpMapTextures;
	unsigned int mCurrentTextureIndex;
	float mLastLeftButtonClickTime;
	float mLastKeyPressTime;
	float mLastMiddleButtonClickTime;

};

typedef Pointer<SpheresController> SpheresControllerPtr;

#endif