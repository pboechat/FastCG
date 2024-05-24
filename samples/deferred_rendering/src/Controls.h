#ifndef CONTROLS_H
#define CONTROLS_H

#include "LightsAnimator.h"

#include <FastCG/World/Behaviour.h>

#include <cstdint>

class Controls : public FastCG::Behaviour
{
    FASTCG_DECLARE_COMPONENT(Controls, FastCG::Behaviour);

protected:
    void OnInstantiate() override
    {
        mpLightsAnimator = GetGameObject()->GetComponent<LightsAnimator>();
    }

    void OnUpdate(float time, float deltaTime) override;

private:
    LightsAnimator *mpLightsAnimator{nullptr};
};

#endif