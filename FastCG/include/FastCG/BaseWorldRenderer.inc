#include <FastCG/BaseWorldRenderer.h>

namespace FastCG
{
    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Initialize()
    {
        mpLightingConstantsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({"Lighting Constants",
                                                                                        BufferType::UNIFORM,
                                                                                        BufferUsage::DYNAMIC,
                                                                                        sizeof(LightingConstants),
                                                                                        &mLightingConstants});
        mpInstanceConstantsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({"Instance Constants",
                                                                                        BufferType::UNIFORM,
                                                                                        BufferUsage::DYNAMIC,
                                                                                        sizeof(InstanceConstants),
                                                                                        &mInstanceConstants});
        mpSceneConstantsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({"Scene Constants",
                                                                                     BufferType::UNIFORM,
                                                                                     BufferUsage::DYNAMIC,
                                                                                     sizeof(SceneConstants),
                                                                                     &mSceneConstants});
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Finalize()
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

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::SetupMaterial(const Material *pMaterial, RenderingContext *pRenderingContext)
    {
        pRenderingContext->Copy(pMaterial->GetMaterialConstantsBuffer(), sizeof(MaterialConstants), &pMaterial->GetMaterialConstants());

        pRenderingContext->Bind(pMaterial->GetShader());
        pRenderingContext->Bind(pMaterial->GetMaterialConstantsBuffer(), Shader::MATERIAL_CONSTANTS_BINDING_INDEX);
        auto *pColorMap = pMaterial->GetColorMap();
        if (pColorMap != nullptr)
        {
            pRenderingContext->Bind(pColorMap, "uColorMap", 0);
        }
        auto *pBumpMap = pMaterial->GetBumpMap();
        if (pBumpMap != nullptr)
        {
            pRenderingContext->Bind(pBumpMap, "uBumpMap", 1);
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateInstanceConstantsBuffer(const glm::mat4 &model, RenderingContext *pRenderingContext)
    {
        auto modelView = mSceneConstants.view * model;

        mInstanceConstants.model = model;
        mInstanceConstants.modelView = modelView;
        mInstanceConstants.modelViewInverseTranspose = glm::transpose(glm::inverse(modelView));
        mInstanceConstants.modelViewProjection = mSceneConstants.projection * modelView;

        pRenderingContext->Copy(mpInstanceConstantsBuffer, sizeof(InstanceConstants), &mInstanceConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstantsBuffer(const PointLight *pPointLight, RenderingContext *pRenderingContext)
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

        pRenderingContext->Copy(mpLightingConstantsBuffer, sizeof(LightingConstants), &mLightingConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstantsBuffer(const DirectionalLight *pDirectionalLight, const glm::vec3 &direction, RenderingContext *pRenderingContext)
    {
        mLightingConstants.light0Position = glm::vec4(direction, -1);
        mLightingConstants.light0DiffuseColor = pDirectionalLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pDirectionalLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pDirectionalLight->GetIntensity();
        mLightingConstants.ambientColor = mArgs.rAmbientLight;

        pRenderingContext->Copy(mpLightingConstantsBuffer, sizeof(LightingConstants), &mLightingConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateSceneConstantsBuffer(const Camera *pCamera, RenderingContext *pRenderingContext)
    {
        mSceneConstants.view = pCamera->GetView();
        mSceneConstants.projection = pCamera->GetProjection();
        mSceneConstants.inverseProjection = glm::inverse(mSceneConstants.projection);
        mSceneConstants.pointSize = 1.0f; // TODO:

        pRenderingContext->Copy(mpSceneConstantsBuffer, sizeof(SceneConstants), &mSceneConstants);
    }

}