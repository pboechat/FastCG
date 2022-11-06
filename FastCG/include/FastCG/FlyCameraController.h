#ifndef FASTCG_FLY_CAMERA_CONTROLLER_H
#define FASTCG_FLY_CAMERA_CONTROLLER_H

#include <FastCG/Behaviour.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FastCG
{
	class FlyCameraController : public Behaviour
	{
		DECLARE_COMPONENT(FlyCameraController, Behaviour);

	public:
		inline float GetWalkSpeed() const
		{
			return mWalkSpeed;
		}

		inline void SetWalkSpeed(float walkSpeed)
		{
			mWalkSpeed = walkSpeed;
		}

		inline float GetTurnSpeed() const
		{
			return mTurnSpeed;
		}

		inline void SetTurnSpeed(float turnSpeed)
		{
			mTurnSpeed = turnSpeed;
		}

		void OnUpdate(float time, float deltaTime) override;

	private:
		float mTheta{0};
		float mPhi{0};
		glm::quat mRotation{};
		glm::uvec2 mLastMousePosition{0, 0};
		bool mRightMouseButtonPressed{false};
		float mWalkSpeed{1};
		float mTurnSpeed{1};
	};

}

#endif