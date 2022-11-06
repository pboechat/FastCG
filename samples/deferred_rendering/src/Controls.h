#ifndef CONTROLS_H
#define CONTROLS_H

#include "LightsAnimator.h"

#include <FastCG/Behaviour.h>

#include <cstdint>

class Controls : public FastCG::Behaviour
{
	DECLARE_COMPONENT(Controls, FastCG::Behaviour);

protected:
	void OnInstantiate() override
	{
		mpLightsAnimator = GetGameObject()->GetComponent<LightsAnimator>();
	}

	void OnUpdate(float time, float deltaTime) override;

private:
	LightsAnimator *mpLightsAnimator{nullptr};
	uint8_t mPressedKeyMask{0};
};

#endif