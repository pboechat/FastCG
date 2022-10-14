#ifndef FASTCG_CAMERA_H_
#define FASTCG_CAMERA_H_

#include <FastCG/Component.h>
#include <FastCG/Transform.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdint>

namespace FastCG
{
	enum class ProjectionMode : uint8_t
	{
		PM_PERSPECTIVE,
		PM_ORTHOGRAPHIC
	};

	class Camera : public Component
	{
		DECLARE_COMPONENT(Camera, Component);

	public:
		inline void OnInstantiate() override
		{
			SetUp();
			mSSAOEnabled = false;
		}

		inline void SetUp(float fieldOfView = 60,
						  float frustumNear = 0.3f,
						  float frustumFar = 1000,
						  float frustumBottom = 0,
						  float frustumTop = 0,
						  float frustumLeft = 0,
						  float frustumRight = 0,
						  ProjectionMode projectionMode = ProjectionMode::PM_PERSPECTIVE)
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
			if (mProjectionMode == ProjectionMode::PM_PERSPECTIVE)
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
			return glm::inverse(GetGameObject()->GetTransform()->GetModel());
		}

		inline void operator=(const Camera &rOther)
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

}

#endif
