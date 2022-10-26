#include <FastCG/OpenGLRenderingSystem.h>
#include <FastCG/OpenGLExceptions.h>

namespace FastCG
{
    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::Initialize()
    {
        CreateUniformBuffers();
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::CreateUniformBuffers()
    {
        mpSceneConstantsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({"Scene Constants",
                                                                                     BufferType::UNIFORM,
                                                                                     BufferUsage::DYNAMIC,
                                                                                     sizeof(SceneConstants),
                                                                                     &mSceneConstants});

        mpInstanceConstantsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({"Instance Constants",
                                                                                        BufferType::UNIFORM,
                                                                                        BufferUsage::DYNAMIC,
                                                                                        sizeof(InstanceConstants),
                                                                                        &mInstanceConstants});

        mpLightingConstantsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({"Lighting Constants",
                                                                                        BufferType::UNIFORM,
                                                                                        BufferUsage::DYNAMIC,
                                                                                        sizeof(LightingConstants),
                                                                                        &mLightingConstants});
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::Finalize()
    {
        DestroyUniformBuffers();
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::DestroyUniformBuffers()
    {
        if (mpSceneConstantsBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpSceneConstantsBuffer);
            mpSceneConstantsBuffer = nullptr;
        }

        if (mpInstanceConstantsBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpInstanceConstantsBuffer);
            mpInstanceConstantsBuffer = nullptr;
        }

        if (mpLightingConstantsBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpLightingConstantsBuffer);
            mpLightingConstantsBuffer = nullptr;
        }
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::UpdateSceneConstantsBuffer(const Camera *pMainCamera)
    {
        mSceneConstants.view = pMainCamera->GetView();
        mSceneConstants.projection = pMainCamera->GetProjection();
        mSceneConstants.inverseProjection = glm::inverse(mSceneConstants.projection);
        mSceneConstants.pointSize = 1.0f; // TODO:

        mpSceneConstantsBuffer->SetSubData(0, sizeof(SceneConstants), &mSceneConstants);
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::UpdateInstanceConstantsBuffer(const glm::mat4 &model)
    {
        auto modelView = mSceneConstants.view * model;

        mInstanceConstants.model = model;
        mInstanceConstants.modelView = modelView;
        mInstanceConstants.modelViewInverseTranspose = glm::transpose(glm::inverse(modelView));
        mInstanceConstants.modelViewProjection = mSceneConstants.projection * modelView;

        mpInstanceConstantsBuffer->SetSubData(0, sizeof(InstanceConstants), &mInstanceConstants);
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::UpdateLightingConstantsBuffer(const PointLight *pPointLight)
    {
        mLightingConstants.light0Position = mSceneConstants.view * glm::vec4(pPointLight->GetGameObject()->GetTransform()->GetPosition(), 1);
        mLightingConstants.light0Position.w = 1;
        mLightingConstants.light0DiffuseColor = pPointLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pPointLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pPointLight->GetIntensity();
        mLightingConstants.light0ConstantAttenuation = pPointLight->GetConstantAttenuation();
        mLightingConstants.light0LinearAttenuation = pPointLight->GetLinearAttenuation();
        mLightingConstants.light0QuadraticAttenuation = pPointLight->GetQuadraticAttenuation();
        mLightingConstants.ambientColor = mArgs.rAmbientLight;

        mpLightingConstantsBuffer->SetSubData(0, sizeof(LightingConstants), &mLightingConstants);
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::UpdateLightingConstantsBuffer(const DirectionalLight *pDirectionalLight, const glm::vec3 &direction)
    {
        mLightingConstants.light0Position = glm::vec4(direction, -1);
        mLightingConstants.light0DiffuseColor = pDirectionalLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pDirectionalLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pDirectionalLight->GetIntensity();
        mLightingConstants.ambientColor = mArgs.rAmbientLight;

        mpLightingConstantsBuffer->SetSubData(0, sizeof(LightingConstants), &mLightingConstants);
    }
}