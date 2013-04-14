#include <Camera.h>
#include <Application.h>
#include <Exception.h>

glm::mat4 Camera::GetProjection() const
{
	if (mProjection == PM_PERSPECTIVE) {
		return glm::perspective(mFieldOfView, Application::GetInstance()->GetAspectRatio(), mNear, mFar);
	} else if (mProjection == PM_ORTHOGRAPHIC) {
		return glm::ortho(mLeft, mRight, mBottom, mTop);
	} else {
		THROW_EXCEPTION(Exception, "Unknown projection: %d", mProjection);
	}
}

glm::mat4 Camera::GetView() const
{
	return glm::lookAt(GetPosition(), GetRotation() * glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
}

