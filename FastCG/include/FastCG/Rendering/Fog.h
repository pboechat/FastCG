#ifndef FASTCG_FOG_H
#define FASTCG_FOG_H

#include <FastCG/Graphics/GraphicsUtils.h>
#include <FastCG/World/Component.h>

namespace FastCG
{
    class Fog : public Component
    {
        FASTCG_DECLARE_COMPONENT(Fog, Component);

    public:
        inline FogMode GetMode() const
        {
            return mMode;
        }

        inline void SetMode(FogMode mode)
        {
            mMode = mode;
        }

        inline const glm::vec4 &GetColor() const
        {
            return mColor;
        }

        inline void SetColor(const glm::vec4 &color)
        {
            mColor = color;
        }

        inline float GetDensity() const
        {
            return mDensity;
        }

        inline void SetDensity(float density)
        {
            mDensity = density;
        }

        inline float GetStart() const
        {
            return mStart;
        }

        inline void SetStart(float start)
        {
            mStart = start;
        }

        inline float GetEnd() const
        {
            return mEnd;
        }

        inline void SetEnd(float end)
        {
            mEnd = end;
        }

    protected:
        FogMode mMode{FogMode::LINEAR};
        glm::vec4 mColor{Colors::WHITE};
        float mDensity{1};
        float mStart{10.0f};
        float mEnd{100.0f};

        void OnRegisterInspectableProperties() override
        {
            RegisterInspectableProperty(
                this, "Mode", &Fog::GetMode, &Fog::SetMode,
                {{FogMode::NONE, "None"}, {FogMode::LINEAR, "Linear"}, {FogMode::EXP, "Exp"}, {FogMode::EXP2, "Exp2"}});
            RegisterInspectableProperty(this, "Color", &Fog::GetColor, &Fog::SetColor, glm::vec4{0, 0, 0, 0},
                                        glm::vec4{1, 1, 1, 1});
            RegisterInspectableProperty(this, "Density", &Fog::GetDensity, &Fog::SetDensity, 0.0f, 1.0f);
            RegisterInspectableProperty(this, "Start", &Fog::GetStart, &Fog::SetStart, 0.0f, 1000.0f);
            RegisterInspectableProperty(this, "End", &Fog::GetEnd, &Fog::SetEnd, 0.0f, 10000.0f);
        }
    };

}

#endif