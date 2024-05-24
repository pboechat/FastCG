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

protected:
    void OnUpdate(float time, float deltaTime) override;

private:
    std::vector<glm::vec3> mDirections;
    float mLastDirectionChangeTime{0};
    bool mInitialized{false};

    void ChangeColors(const std::vector<FastCG::PointLight *> &rPointLights);
};

#endif