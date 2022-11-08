#include <FastCG/BaseWorldRenderer.h>

namespace
{
    const auto *CreateShadowMapTexture(uint32_t width, uint32_t height, FastCG::TextureFilter filter = FastCG::TextureFilter::LINEAR_FILTER, const void *pData = nullptr)
    {
        return FastCG::RenderingSystem::GetInstance()->CreateTexture({"Shadow Map",
                                                                      width,
                                                                      height,
                                                                      FastCG::TextureType::TEXTURE_2D,
                                                                      FastCG::TextureFormat::DEPTH,
                                                                      {24},
                                                                      FastCG::TextureDataType::FLOAT,
                                                                      filter,
                                                                      FastCG::TextureWrapMode::CLAMP,
                                                                      false,
                                                                      pData});
    }
}

namespace FastCG
{
    const glm::mat4 INVALID_SHADOW_MAP_PROJECTION = glm::ortho(-FLT_MAX, FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX, FLT_MAX);

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    typename BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::ShadowMapKey BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetShadowMapKey(const Light *pLight) const
    {
        return (ShadowMapKey)pLight;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Initialize()
    {
        mpLightingConstantsBuffer = RenderingSystem::GetInstance()->CreateBuffer({"Lighting Constants",
                                                                                  BufferType::UNIFORM,
                                                                                  BufferUsage::DYNAMIC,
                                                                                  sizeof(LightingConstants),
                                                                                  &mLightingConstants});
        mpInstanceConstantsBuffer = RenderingSystem::GetInstance()->CreateBuffer({"Instance Constants",
                                                                                  BufferType::UNIFORM,
                                                                                  BufferUsage::DYNAMIC,
                                                                                  sizeof(InstanceConstants),
                                                                                  &mInstanceConstants});
        mpSceneConstantsBuffer = RenderingSystem::GetInstance()->CreateBuffer({"Scene Constants",
                                                                               BufferType::UNIFORM,
                                                                               BufferUsage::DYNAMIC,
                                                                               sizeof(SceneConstants),
                                                                               &mSceneConstants});
        float data[] = {1};
        mpEmptyShadowMap = CreateShadowMapTexture(1, 1, TextureFilter::POINT_FILTER, data);
        mpShadowMapPassShader = RenderingSystem::GetInstance()->FindShader("ShadowMapPass");
        assert(mpShadowMapPassShader != nullptr);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Finalize()
    {
        mpShadowMapPassShader = nullptr;

        if (mpEmptyShadowMap != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyTexture(mpEmptyShadowMap);
            mpEmptyShadowMap = nullptr;
        }

        if (mpSceneConstantsBuffer != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyBuffer(mpSceneConstantsBuffer);
            mpSceneConstantsBuffer = nullptr;
        }

        if (mpInstanceConstantsBuffer != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyBuffer(mpInstanceConstantsBuffer);
            mpInstanceConstantsBuffer = nullptr;
        }

        if (mpLightingConstantsBuffer != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyBuffer(mpLightingConstantsBuffer);
            mpLightingConstantsBuffer = nullptr;
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const typename BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::ShadowMap &BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetOrCreateShadowMap(const Light *pLight)
    {
        auto shadowMapKey = GetShadowMapKey(pLight);
        auto it = mShadowMaps.find(shadowMapKey);
        if (it == mShadowMaps.end())
        {
            it = mShadowMaps.emplace(shadowMapKey, ShadowMap{pLight, CreateShadowMapTexture(mArgs.rScreenWidth, mArgs.rScreenHeight)}).first;
        }
        return it->second;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    bool BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetShadowMap(const Light *pLight, ShadowMap &rShadowMap) const
    {
        auto shadowMapKey = GetShadowMapKey(pLight);
        auto it = mShadowMaps.find(shadowMapKey);
        if (it == mShadowMaps.end())
        {
            return false;
        }
        rShadowMap = it->second;
        return true;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GenerateShadowMaps(RenderingContext *pRenderingContext)
    {
        auto &rShadowCastersRenderBatch = mArgs.rRenderBatches[0];
        assert(rShadowCastersRenderBatch.type == RenderBatchType::SHADOW_CASTERS);
        if (rShadowCastersRenderBatch.renderables.empty())
        {
            return;
        }

        pRenderingContext->PushDebugMarker("Shadow Casters Passes");
        {
            pRenderingContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
            pRenderingContext->SetDepthTest(true);
            pRenderingContext->SetDepthWrite(true);
            pRenderingContext->SetStencilTest(false);
            pRenderingContext->SetScissorTest(false);
            pRenderingContext->SetCullMode(Face::BACK);
            pRenderingContext->SetBlend(false);

            pRenderingContext->Bind(mpShadowMapPassShader);

            for (const auto *pRenderable : rShadowCastersRenderBatch.renderables)
            {
                if (!pRenderable->GetGameObject()->IsActive())
                {
                    continue;
                }

                const auto *pMesh = pRenderable->GetMesh();

                pRenderingContext->SetVertexBuffers(pMesh->GetVertexBuffers(), pMesh->GetVertexBufferCount());
                pRenderingContext->SetIndexBuffer(pMesh->GetIndexBuffer());

                auto model = pRenderable->GetGameObject()->GetTransform()->GetModel();

                for (size_t i = 0; i < mArgs.rDirectionalLights.size(); ++i)
                {
                    const auto *pDirectionalLight = mArgs.rDirectionalLights[i];

                    const auto &rShadowMap = GetOrCreateShadowMap(pDirectionalLight);

                    const auto *pShadowMapTexture = rShadowMap.GetTexture();

                    pRenderingContext->SetRenderTargets(&pShadowMapTexture, 1);
                    pRenderingContext->ClearDepthTarget(0, 1);

                    UpdateInstanceConstants(model, rShadowMap.GetView(), rShadowMap.GetProjection(), pRenderingContext);

                    pRenderingContext->Bind(mpInstanceConstantsBuffer, OpenGLShader::INSTANCE_CONTANTS_BINDING_INDEX);

                    pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, pMesh->GetIndexCount(), 0, 0);

                    mArgs.rRenderingStatistics.drawCalls++;
                }
            }
        }
        pRenderingContext->PopDebugMarker();
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
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateInstanceConstants(const glm::mat4 &rModel, const glm::mat4 &rView, const glm::mat4 &rProjection, RenderingContext *pRenderingContext)
    {
        auto modelView = rView * rModel;

        mInstanceConstants.model = rModel;
        mInstanceConstants.modelView = modelView;
        mInstanceConstants.modelViewInverseTranspose = glm::transpose(glm::inverse(modelView));
        mInstanceConstants.modelViewProjection = rProjection * modelView;

        pRenderingContext->Copy(mpInstanceConstantsBuffer, sizeof(InstanceConstants), &mInstanceConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstants(const PointLight *pPointLight, const glm::mat4 &rView, RenderingContext *pRenderingContext)
    {
        mLightingConstants.light0Position = glm::vec4(pPointLight->GetGameObject()->GetTransform()->GetPosition(), 1);
        mLightingConstants.light0ViewPosition = rView * mLightingConstants.light0Position;
        mLightingConstants.light0DiffuseColor = pPointLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pPointLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pPointLight->GetIntensity();
        mLightingConstants.light0ConstantAttenuation = pPointLight->GetConstantAttenuation();
        mLightingConstants.light0LinearAttenuation = pPointLight->GetLinearAttenuation();
        mLightingConstants.light0QuadraticAttenuation = pPointLight->GetQuadraticAttenuation();
        mLightingConstants.ambientColor = mArgs.rAmbientLight;

        // TODO:
        mLightingConstants.shadowMapViewProjection = INVALID_SHADOW_MAP_PROJECTION;
        mLightingConstants.shadowMapBias = 0;
        pRenderingContext->Bind(mpEmptyShadowMap, "uShadowMap", 2);

        pRenderingContext->Copy(mpLightingConstantsBuffer, sizeof(LightingConstants), &mLightingConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstants(const DirectionalLight *pDirectionalLight, const glm::vec3 &rDirection, RenderingContext *pRenderingContext)
    {
        mLightingConstants.light0Position = glm::vec4(rDirection, 0);
        mLightingConstants.light0ViewPosition = mLightingConstants.light0Position;
        mLightingConstants.light0DiffuseColor = pDirectionalLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pDirectionalLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pDirectionalLight->GetIntensity();
        mLightingConstants.ambientColor = mArgs.rAmbientLight;

        ShadowMap shadowMap;
        if (GetShadowMap(pDirectionalLight, shadowMap))
        {
            mLightingConstants.shadowMapViewProjection = shadowMap.GetProjection() * shadowMap.GetView();
            mLightingConstants.shadowMapBias = mShadowMapBias;

            pRenderingContext->Bind(shadowMap.GetTexture(), "uShadowMap", 2);
        }
        else
        {
            mLightingConstants.shadowMapViewProjection = INVALID_SHADOW_MAP_PROJECTION;
            mLightingConstants.shadowMapBias = 0;
            pRenderingContext->Bind(mpEmptyShadowMap, "uShadowMap", 2);
        }

        pRenderingContext->Copy(mpLightingConstantsBuffer, sizeof(LightingConstants), &mLightingConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateSceneConstants(const glm::mat4 &rView, const glm::mat4 &rProjection, RenderingContext *pRenderingContext)
    {
        mSceneConstants.view = rView;
        mSceneConstants.inverseView = glm::inverse(rView);
        mSceneConstants.projection = rProjection;
        mSceneConstants.inverseProjection = glm::inverse(rProjection);
        mSceneConstants.pointSize = 1.0f; // TODO:

        pRenderingContext->Copy(mpSceneConstantsBuffer, sizeof(SceneConstants), &mSceneConstants);
    }

}