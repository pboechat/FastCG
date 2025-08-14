#ifndef CONTROLS_H
#define CONTROLS_H

#include <FastCG/World/Behaviour.h>

using namespace FastCG;

class Controls : public Behaviour
{
    FASTCG_DECLARE_COMPONENT(Controls, Behaviour);

protected:
    void OnUpdate(float time, float deltaTime) override;
};

#endif