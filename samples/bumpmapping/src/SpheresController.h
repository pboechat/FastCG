#ifndef SPHERESCONTROLLER_H_
#define SPHERESCONTROLLER_H_

#include <Updateable.h>
#include <TriangleMesh.h>
#include <Texture.h>

#include <glm/glm.hpp>

#include <vector>

class SpheresController : public Updateable
{
public:
	SpheresController(const std::vector<TriangleMeshPtr>& rSpheresPtrs, float rotationSpeed, const glm::vec3& rRotationAxis, const std::vector<TexturePtr>& rColorMapTexturesPtrs, const std::vector<TexturePtr>& rBumpMapTexturesPtrs);

	virtual void Update(float time, float deltaTime);

private:
	std::vector<TriangleMeshPtr> mSpheresPtrs;
	float mRotationSpeed;
	glm::vec3 mRotationAxis;
	std::vector<TexturePtr> mColorMapTexturePtrs;
	std::vector<TexturePtr> mBumpMapTexturePtrs;
	unsigned int mCurrentTextureIndex;
	float mLastLeftButtonClickTime;
	float mLastF1PressTime;
	float mLastMiddleButtonClickTime;

};

#endif