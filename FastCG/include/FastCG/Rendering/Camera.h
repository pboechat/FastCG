#ifndef FASTCG_CAMERA_H
#define FASTCG_CAMERA_H

#include <FastCG/World/Component.h>
#include <FastCG/World/Transform.h>
#include <FastCG/World/WorldSystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdint>

namespace FastCG
{
    enum class ProjectionMode : uint8_t
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    struct CameraSetupArgs
    {
        union {
            struct
            {
                float nearClip;
                float farClip;
                float fieldOfView;
                float aspectRatio;
                float padding[2];
            } perspective{};
            struct
            {
                float nearClip;
                float farClip;
                float left;
                float top;
                float right;
                float bottom;
            } orthographic;
        };
    };

    class Camera : public Component
    {
        FASTCG_DECLARE_COMPONENT(Camera, Component);

    public:
        inline bool IsSSAOEnabled() const
        {
            return mSSAOEnabled;
        }

        inline void SetSSAOEnabled(bool ssaoEnabled)
        {
            mSSAOEnabled = ssaoEnabled;
        }

        inline glm::mat4 GetProjection() const
        {
            if (mProjectionMode == ProjectionMode::PERSPECTIVE)
            {
                return glm::perspective(GetFieldOfViewInRadians(), GetAspectRatio(), GetNearClip(), GetFarClip());
            }
            else
            {
                return glm::ortho(GetLeft(), GetRight(), GetBottom(), GetTop(), GetNearClip(), GetFarClip());
            }
        }

        inline glm::mat4 GetView() const
        {
            return glm::inverse(GetGameObject()->GetTransform()->GetModel());
        }

        inline float GetFieldOfViewInRadians() const
        {
            return mArgs.perspective.fieldOfView;
        }

        inline float GetFieldOfView() const
        {
            return glm::degrees(mArgs.perspective.fieldOfView);
        }

        inline void SetFieldOfView(float fieldOfView)
        {
            // store FOV in radians
            mArgs.perspective.fieldOfView = glm::radians(fieldOfView);
        }

        inline float GetAspectRatio() const
        {
            return mArgs.perspective.aspectRatio;
        }

        inline void SetAspectRatio(float aspectRatio)
        {
            mArgs.perspective.aspectRatio = aspectRatio;
        }

        inline float GetNearClip() const
        {
            return mArgs.perspective.nearClip;
        }

        inline void SetNearClip(float nearClip)
        {
            mArgs.perspective.nearClip = nearClip;
        }

        inline float GetFarClip() const
        {
            return mArgs.perspective.farClip;
        }

        inline void SetFarClip(float farClip)
        {
            mArgs.perspective.farClip = farClip;
        }

        inline float GetLeft() const
        {
            return mArgs.orthographic.left;
        }

        inline void SetLeft(float left)
        {
            mArgs.orthographic.left = left;
        }

        inline float GetTop() const
        {
            return mArgs.orthographic.top;
        }

        inline void SetTop(float top)
        {
            mArgs.orthographic.top = top;
        }

        inline float GetRight() const
        {
            return mArgs.orthographic.right;
        }

        inline void SetRight(float right)
        {
            mArgs.orthographic.right = right;
        }

        inline float GetBottom() const
        {
            return mArgs.orthographic.bottom;
        }

        inline void SetBottom(float bottom)
        {
            mArgs.orthographic.bottom = bottom;
        }

        inline ProjectionMode GetProjectionMode() const
        {
            return mProjectionMode;
        }

        inline void SetProjectionMode(ProjectionMode projectionMode)
        {
            mProjectionMode = projectionMode;
        }

        inline void operator=(const Camera &rOther)
        {
            mArgs = rOther.mArgs;
            mProjectionMode = rOther.mProjectionMode;
        }

    protected:
        void OnInstantiate() override
        {
            WorldSystem::GetInstance()->RegisterCamera(this);
        }

        void OnRegisterInspectableProperties() override
        {
            RegisterInspectableProperty(
                this, "Projection Mode", &Camera::GetProjectionMode, &Camera::SetProjectionMode,
                {{ProjectionMode::ORTHOGRAPHIC, "Orthographic"}, {ProjectionMode::PERSPECTIVE, "Perspective"}});
            RegisterInspectableProperty(this, "Field Of View", &Camera::GetFieldOfView, &Camera::SetFieldOfView, 1.0f,
                                        179.0f);
            RegisterInspectableProperty(this, "Aspect Ratio", &Camera::GetAspectRatio, &Camera::SetAspectRatio, 0.1f,
                                        10.0f);
            RegisterInspectableProperty(this, "Near Clip", &Camera::GetNearClip, &Camera::SetNearClip, 0.1f, 10.0f);
            RegisterInspectableProperty(this, "Far Clip", &Camera::GetFarClip, &Camera::SetFarClip, 10.0f, 1000.0f);
            RegisterInspectableProperty(this, "SSAO Enabled", &Camera::IsSSAOEnabled, &Camera::SetSSAOEnabled);
        }

        void OnDestroy() override
        {
            WorldSystem::GetInstance()->UnregisterCamera(this);
        }

    private:
        CameraSetupArgs mArgs;
        ProjectionMode mProjectionMode;
        bool mSSAOEnabled;

        Camera(GameObject *pGameObject, const CameraSetupArgs &rArgs, ProjectionMode projectionMode,
               bool ssaoEnabled = false)
            : Component(pGameObject), mArgs(rArgs), mProjectionMode(projectionMode), mSSAOEnabled(ssaoEnabled)
        {
            SetFieldOfView(rArgs.perspective.fieldOfView);
        }
    };

}

#endif
