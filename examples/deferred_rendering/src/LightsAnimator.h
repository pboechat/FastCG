#ifndef LIGHTS_ANIMATOR_H
#define LIGHTS_ANIMATOR_H

#include <FastCG/Rendering/PointLight.h>
#include <FastCG/World/Behaviour.h>

#include <glm/glm.hpp>

#include <vector>

class LightsAnimator : public FastCG::Behaviour
{
    FASTCG_DECLARE_COMPONENT(LightsAnimator, FastCG::Behaviour);

public:
    void ChangeColors();
    bool IsEnabled() const
    {
        return mEnabled;
    }
    void SetEnabled(bool value)
    {
        mEnabled = value;
    }

protected:
    void OnUpdate(float time, float deltaTime) override;
    void OnRegisterInspectableProperties() override
    {
        RegisterInspectableProperty(this, "Enabled", &LightsAnimator::IsEnabled, &LightsAnimator::SetEnabled);
    }

private:
    std::vector<glm::vec3> mDirections;
    std::vector<FastCG::PointLight *> mPointLights;
    float mLastDirectionChangeTime{0};
    bool mInitialized{false};
    bool mEnabled{false};
};

#endif