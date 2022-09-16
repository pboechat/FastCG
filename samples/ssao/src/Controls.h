#ifndef CONTROLS_H_
#define CONTROLS_H_

#include <FastCG/Behaviour.h>

#include <vector>

using namespace FastCG;

class Controls : public Behaviour
{
	DECLARE_COMPONENT(Controls, Behaviour);

protected:
	void OnUpdate(float time, float deltaTime) override;

	void OnInstantiate() override
	{
		mLastKeyPressTime = 0;
	}

private:
	float mLastKeyPressTime;

};

#endif