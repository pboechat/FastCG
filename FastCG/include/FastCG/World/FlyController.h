#ifndef FASTCG_FLY_CONTROLLER_H
#define FASTCG_FLY_CONTROLLER_H

#include <FastCG/World/Behaviour.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FastCG
{
    class FlyController : public Behaviour
    {
        FASTCG_DECLARE_COMPONENT(FlyController, Behaviour);

    public:
        inline float GetMoveSpeed() const
        {
            return mMoveSpeed;
        }

        inline void SetMoveSpeed(float walkSpeed)
        {
            mMoveSpeed = walkSpeed;
        }

        inline float GetTurnSpeed() const
        {
            return mTurnSpeed;
        }

        inline void SetTurnSpeed(float turnSpeed)
        {
            mTurnSpeed = turnSpeed;
        }

        void OnUpdate(float time, float deltaTime) override;

    protected:
        void OnRegisterInspectableProperties() override
        {
            RegisterInspectableProperty(this, "Move Speed", &FlyController::GetMoveSpeed, &FlyController::SetMoveSpeed,
                                        0.0f, 100.0f);
            RegisterInspectableProperty(this, "Turn Speed", &FlyController::GetTurnSpeed, &FlyController::SetTurnSpeed,
                                        0.0f, 100.0f);
        }

    private:
        float mTheta{0};
        float mPhi{0};
        glm::quat mRotation{1, 0, 0, 0};
        glm::uvec2 mLastMousePosition{0, 0};
        bool mRightMouseButtonPressed{false};
        float mMoveSpeed{1};
        float mTurnSpeed{1};
    };

}

#endif