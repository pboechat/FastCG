#ifndef CONTROLS_H
#define CONTROLS_H

#include <FastCG/Behaviour.h>

#include <cstdint>

using namespace FastCG;

class Controls : public Behaviour
{
	DECLARE_COMPONENT(Controls, Behaviour);

public:
	void OnUpdate(float time, float deltaTime) override;

private:
	uint8_t mPressedKeyMask{0};
};

#endif