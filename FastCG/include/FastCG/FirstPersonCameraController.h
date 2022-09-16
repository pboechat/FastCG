#ifndef FASTCG_FIRST_PERSON_CAMERA_CONTROLLER_H_
#define FASTCG_FIRST_PERSON_CAMERA_CONTROLLER_H_

#include <FastCG/Behaviour.h>

#include <glm/glm.hpp>

namespace FastCG
{
	class FirstPersonCameraController : public Behaviour
	{
		DECLARE_COMPONENT(FirstPersonCameraController, Behaviour);

	public:
		static const float PITCH_LIMIT;

		void OnInstantiate() override
		{
			mRightMouseButtonPressed = false;
			mFlying = false;
		}

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

		inline bool IsFlying() const
		{
			return mFlying;
		}

		inline void SetFlying(bool flying)
		{
			mFlying = flying;
		}

		void OnUpdate(float time, float deltaTime) override;

	private:
		glm::vec2 mLastMousePosition;
		bool mRightMouseButtonPressed;
		float mWalkSpeed;
		float mTurnSpeed;
		bool mFlying;

	};

}

#endif