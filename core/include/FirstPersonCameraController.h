#ifndef FIRSTPERSONCAMERACONTROLLER_H_
#define FIRSTPERSONCAMERACONTROLLER_H_

#include <Behaviour.h>

#include <glm/glm.hpp>

class FirstPersonCameraController : public Behaviour
{
	DECLARE_TYPE;

public:
	FirstPersonCameraController() :
		mRightMouseButtonPressed(false)
	{
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

	virtual void OnUpdate(float time, float deltaTime);

private:
	glm::vec2 mLastMousePosition;
	bool mRightMouseButtonPressed;
	float mWalkSpeed;
	float mTurnSpeed;
	
};

typedef Pointer<FirstPersonCameraController> FirstPersonCameraControllerPtr;

#endif