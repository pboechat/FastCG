#ifndef LIGHTS_ANIMATOR_H
#define LIGHTS_ANIMATOR_H

#include <FastCG/Behaviour.h>

#include <vector>

class LightsAnimator : public FastCG::Behaviour
{
    DECLARE_COMPONENT(LightsAnimator, FastCG::Behaviour);

public:
    inline void SetLights(const std::vector<FastCG::GameObject *> &rSceneLights)
    {
        mSceneLights = rSceneLights;
    }

protected:
    void OnUpdate(float time, float deltaTime) override;

private:
    std::vector<FastCG::GameObject *> mSceneLights;
};

#endif