#ifndef CAMERA_H_
#define CAMERA_H_

#include <Component.h>
#include <Transform.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum ProjectionMode
{
	PM_PERSPECTIVE,
	PM_ORTHOGRAPHIC
};

COMPONENT(Camera, Component)
public:
	inline virtual void OnInstantiate()
	{
		SetUp();
		mSSAOEnabled = false;
	}

	inline void SetUp(float fieldOfView = 60.0f, float frustumNear = 0.3f, float frustumFar = 1000.0f, float frustumBottom = 0.0f, float frustumTop = 0.0f, float frustumLeft = 0.0f, float frustumRight = 0.0f, ProjectionMode projectionMode = PM_PERSPECTIVE)
	{
		mFieldOfView = fieldOfView;
		mFrustumNear = frustumNear;
		mFrustumFar = frustumFar;
		mFrustumBottom = frustumBottom;
		mFrustumTop = frustumTop;
		mFrustumLeft = frustumLeft;
		mFrustumRight = frustumRight;
		mProjectionMode = projectionMode;
	}

	inline bool IsSSAOEnabled() const
	{
		return mSSAOEnabled;
	}

	inline void SetSSAOEnabled(bool ssaoEnabled)
	{
		mSSAOEnabled = ssaoEnabled;
	}

	inline void SetAspectRatio(float aspectRatio)
	{
		mAspectRatio = aspectRatio;
	}

	inline glm::mat4 GetProjection() const
	{
		if (mProjectionMode == PM_PERSPECTIVE)
		{
			return glm::perspective(mFieldOfView, mAspectRatio, mFrustumNear, mFrustumFar);
		}

		else
		{
			return glm::ortho(mFrustumLeft, mFrustumRight, mFrustumBottom, mFrustumTop, mFrustumNear, mFrustumFar);
		}
	}

	inline glm::mat4 GetView() const
	{
		const Transform* pTransform = GetGameObject()->GetTransform();
		return glm::lookAt(pTransform->GetPosition(), pTransform->GetPosition() + pTransform->GetForward(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	inline void operator =(const Camera& rOther)
	{
		mFieldOfView = rOther.mFieldOfView;
		mFrustumNear = rOther.mFrustumNear;
		mFrustumFar = rOther.mFrustumFar;
		mFrustumTop = rOther.mFrustumTop;
		mFrustumBottom = rOther.mFrustumBottom;
		mFrustumLeft = rOther.mFrustumLeft;
		mFrustumRight = rOther.mFrustumRight;
		mProjectionMode = rOther.mProjectionMode;
	}
	
	inline float GetFieldOfView() const
	{
		return mFieldOfView;
	}

private:
	float mFieldOfView;
	float mFrustumNear;
	float mFrustumFar;
	float mFrustumTop;
	float mFrustumBottom;
	float mFrustumLeft;
	float mFrustumRight;
	float mAspectRatio;
	ProjectionMode mProjectionMode;
	bool mSSAOEnabled;

};

#endif
