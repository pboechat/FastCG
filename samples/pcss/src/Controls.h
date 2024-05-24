#ifndef CONTROLS_H
#define CONTROLS_H

#include <FastCG/World/Behaviour.h>

#include <cstdint>

class Controls : public FastCG::Behaviour
{
    FASTCG_DECLARE_COMPONENT(Controls, FastCG::Behaviour);

protected:
    void OnUpdate(float time, float deltaTime) override;
};

#endif