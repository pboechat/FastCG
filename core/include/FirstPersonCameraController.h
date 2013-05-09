#ifndef FIRSTPERSONCAMERACONTROLLER_H_
#define FIRSTPERSONCAMERACONTROLLER_H_

#include <Updateable.h>

#include <glm/glm.hpp>

class FirstPersonCameraController : public Updateable
{
public:
	FirstPersonCameraController(float walkSpeed = 1.0f, float turnSpeed = 1.0f) :
		mWalkSpeed(walkSpeed),
		mTurnSpeed(turnSpeed),
		mRightMouseButtonPressed(false)
	{
	}

	virtual void Update(float time, float deltaTime);

private:
	glm::vec2 mLastMousePosition;
	bool mRightMouseButtonPressed;
	float mWalkSpeed;
	float mTurnSpeed;

};

#endif