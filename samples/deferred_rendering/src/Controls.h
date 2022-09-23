#ifndef CONTROLS_H_
#define CONTROLS_H_

#include <FastCG/Behaviour.h>

#include <cstdint>

using namespace FastCG;

class Controls : public Behaviour
{
	DECLARE_COMPONENT(Controls, Behaviour);

public:
	void OnUpdate(float time, float deltaTime) override;

private:
	uint8_t mPressedKeyMask{ 0 };

};

#endif