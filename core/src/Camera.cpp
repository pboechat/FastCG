#include <Camera.h>
#include <Application.h>
#include <Exception.h>

Camera::~Camera()
{
}

glm::mat4 Camera::GetProjection() const
{
	if (mProjection == PM_PERSPECTIVE)
	{
		return glm::perspective(mFieldOfView, Application::GetInstance()->GetAspectRatio(), mNear, mFar);
	}

	else if (mProjection == PM_ORTHOGRAPHIC)
	{
		return glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}

	else
	{
		THROW_EXCEPTION(Exception, "Unknown projection: %d", mProjection);
	}
}

glm::mat4 Camera::GetView() const
{
	return glm::lookAt(GetPosition(), GetForward(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::operator =(const Camera& rOther)
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
