#ifndef CONTROLS_H
#define CONTROLS_H

#include <FastCG/Behaviour.h>

#include <vector>
#include <cstdint>

using namespace FastCG;

class Controls : public Behaviour
{
	DECLARE_COMPONENT(Controls, Behaviour);

protected:
	void OnUpdate(float time, float deltaTime) override;

private:
	uint8_t mPressedKeyMask{0};
};

#endif