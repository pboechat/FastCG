#ifndef FIRSTPERSONCAMERACONTROLLER_H_
#define FIRSTPERSONCAMERACONTROLLER_H_

#include <Behaviour.h>

#include <glm/glm.hpp>

COMPONENT(FirstPersonCameraController, Behaviour)
public:
	static const float PITCH_LIMIT;

	virtual void OnInstantiate()
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

	virtual void OnUpdate(float time, float deltaTime);

private:
	glm::vec2 mLastMousePosition;
	bool mRightMouseButtonPressed;
	float mWalkSpeed;
	float mTurnSpeed;
	bool mFlying;
	
};

#endif