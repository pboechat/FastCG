#ifndef LIGHTS_ANIMATOR_H
#define LIGHTS_ANIMATOR_H

#include <FastCG/Behaviour.h>

#include <vector>

class LightsAnimator : public FastCG::Behaviour
{
    FASTCG_DECLARE_COMPONENT(LightsAnimator, FastCG::Behaviour);

public:
    inline void SetLights(const std::vector<FastCG::GameObject *> &rSceneLights)
    {
        mSceneLights = rSceneLights;
    }

    inline bool IsEnabled() const
    {
        return mEnabled;
    }

    inline void SetEnabled(bool enabled)
    {
        mEnabled = enabled;
    }

protected:
    void OnUpdate(float time, float deltaTime) override;

    void OnRegisterInspectableProperties() override
    {
        RegisterInspectableProperty(this, "Enabled", &LightsAnimator::IsEnabled, &LightsAnimator::SetEnabled);
    }

private:
    bool mEnabled{true};
    std::vector<FastCG::GameObject *> mSceneLights;
};

#endif