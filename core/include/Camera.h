#ifndef CAMERA_H_
#define CAMERA_H_

#include "include/Transformable.h"
#include "include/Application.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum ProjectionMode
{
	PM_PERSPECTIVE, PM_ORTHOGRAPHIC
};

class Camera : public Transformable
{
public:
	Camera(float fieldOfView = 45.0f, float near = 0.3f, float far = 1000.0f, float top = 0.0f, float bottom = 0.0f, float left = 0.0f, float right = 0.0f, ProjectionMode projection = PM_PERSPECTIVE) :
			mFieldOfView(fieldOfView), mNear(near), mFar(far), mTop(top), mBottom(bottom), mLeft(left), mRight(right), mProjection(projection)
	{

	}
	virtual ~Camera()
	{
	}

	inline glm::mat4 GetPerspective() const
	{
		return glm::perspective(mFieldOfView, Application::GetInstance()->GetAspectRatio(), mNear, mFar);
	}

	inline glm::mat4 GetView() const
	{
		return glm::lookAt(mPosition, mRotation * glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
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
