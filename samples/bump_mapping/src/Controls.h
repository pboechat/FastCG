#ifndef CONTROLS_H
#define CONTROLS_H

#include <FastCG/Behaviour.h>

using namespace FastCG;

class Controls : public Behaviour
{
	FASTCG_DECLARE_COMPONENT(Controls, Behaviour);

protected:
	void OnUpdate(float time, float deltaTime) override;

private:
	uint8_t mPressedKeyMask{0};
};

#endif