#ifndef CAMERA_H_
#define CAMERA_H_

#include <Transformable.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum ProjectionMode
{
	PM_PERSPECTIVE,
	PM_ORTHOGRAPHIC
};

class Application;

class Camera: public Transformable
{
public:
	Camera(float fieldOfView = 45.0f, float near = 0.3f, float far = 1000.0f, float bottom = 0.0f, float top = 0.0f, float left = 0.0f, float right = 0.0f, ProjectionMode projection = PM_PERSPECTIVE) :
		mFieldOfView(fieldOfView), mNear(near), mFar(far), mBottom(bottom), mTop(top), mLeft(left), mRight(right), mProjection(projection)
	{
	}

	virtual ~Camera()
	{
	}

	glm::mat4 GetProjection() const;
	glm::mat4 GetView() const;

	void operator =(const Camera& rOther)
	{
		mFieldOfView = rOther.mFieldOfView;
		mNear = rOther.mNear;
		mFar = rOther.mFar;
		mTop = rOther.mTop;
		mBottom = rOther.mBottom;
		mLeft = rOther.mLeft;
		mRight = rOther.mRight;
		mProjection = rOther.mProjection;
	}

private:
	float mFieldOfView;
	float mNear;
	float mFar;
	float mTop;
	float mBottom;
	float mLeft;
	float mRight;
	ProjectionMode mProjection;

};

#endif
