#include <Camera.h>

IMPLEMENT_TYPE(Camera, Component);

/*glm::mat4 Camera::GetProjection() const
{
	if (mProjectionMode == PM_PERSPECTIVE)
	{
		return glm::perspective(mFieldOfView, mAspectRatio, mFrustumNear, mFrustumFar);
	}

	else if (mProjectionMode == PM_ORTHOGRAPHIC)
	{
		return glm::ortho(mFrustumLeft, mFrustumRight, mFrustumBottom, mFrustumTop, mFrustumNear, mFrustumFar);
	}

	else
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "Unknown projection: %d", mProjectionMode);
	}
}*/

/*glm::mat4 Camera::GetView() const
{
	const Transform* pTransform = GetGameObject()->GetTransform();
	return glm::lookAt(pTransform->GetPosition(), pTransform->GetForward(), glm::vec3(0.0f, 1.0f, 0.0f));
}*/

/*void Camera::SetUp(float fieldOfView, float frustumNear, float frustumFar, float frustumBottom, float frustumTop, float frustumLeft, float frustumRight, ProjectionMode projectionMode)
{
	mFieldOfView = fieldOfView;
	mFrustumNear = frustumNear;
	mFrustumFar = frustumFar;
	mFrustumBottom = frustumBottom;
	mFrustumTop = frustumTop;
	mFrustumLeft = frustumLeft;
	mFrustumRight = frustumRight;
	mProjectionMode = projectionMode;
}*/

/*void Camera::operator =(const Camera& rOther)
{
	mFieldOfView = rOther.mFieldOfView;
	mFrustumNear = rOther.mFrustumNear;
	mFrustumFar = rOther.mFrustumFar;
	mFrustumTop = rOther.mFrustumTop;
	mFrustumBottom = rOther.mFrustumBottom;
	mFrustumLeft = rOther.mFrustumLeft;
	mFrustumRight = rOther.mFrustumRight;
	mProjectionMode = rOther.mProjectionMode;
}*/
