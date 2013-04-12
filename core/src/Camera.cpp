#include <Camera.h>
#include <Application.h>

glm::mat4 Camera::GetPerspective() const
{
	return glm::perspective(mFieldOfView, Application::GetInstance()->GetAspectRatio(), mNear, mFar);
}

glm::mat4 Camera::GetView() const
{
	return glm::lookAt(mPosition, mRotation * glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
}

