#ifndef FASTCG_POINT_LIGHT_H
#define FASTCG_POINT_LIGHT_H

#include <FastCG/Rendering/Light.h>

namespace FastCG
{
    class PointLight : public Light
    {
        FASTCG_DECLARE_COMPONENT(PointLight, Light);

    public:
        inline float GetConstantAttenuation() const
        {
            return mConstantAttenuation;
        }

        inline void SetConstantAttenuation(float constantAttenuation)
        {
            mConstantAttenuation = constantAttenuation;
        }

        inline float GetLinearAttenuation() const
        {
            return mLinearAttenuation;
        }

        inline void SetLinearAttenuation(float linearAttenuation)
        {
            mLinearAttenuation = linearAttenuation;
        }

        inline float GetQuadraticAttenuation() const
        {
            return mQuadraticAttenuation;
        }

        inline void SetQuadraticAttenuation(float quadraticAttenuation)
        {
            mQuadraticAttenuation = quadraticAttenuation;
        }

    protected:
        void OnRegisterInspectableProperties() override
        {
            Light::OnRegisterInspectableProperties();
            RegisterInspectableProperty(this, "Constant Attenuation", &PointLight::GetConstantAttenuation,
                                        &PointLight::SetConstantAttenuation, 0.0f, 100.0f);
            RegisterInspectableProperty(this, "Linear Attenuation", &PointLight::GetLinearAttenuation,
                                        &PointLight::SetLinearAttenuation, 0.0f, 100.0f);
            RegisterInspectableProperty(this, "Quadratic Attenuation", &PointLight::GetQuadraticAttenuation,
                                        &PointLight::SetQuadraticAttenuation, 0.0f, 100.0f);
        }

    private:
        float mConstantAttenuation{0};
        float mLinearAttenuation{0};
        float mQuadraticAttenuation{1};
    };

}

#endif