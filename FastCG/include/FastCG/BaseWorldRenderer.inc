#include <FastCG/StandardGeometries.h>
#include <FastCG/Random.h>
#include <FastCG/MathT.h>
#include <FastCG/BaseWorldRenderer.h>

namespace
{
    template <size_t N>
    void GenerateRandomSamplesInAHemisphere(glm::vec4 randomSamples[N])
    {
        FastCG::Random::SeedWithTime();
        for (uint32_t i = 0; i < N; i++)
        {
            glm::vec3 randomSample{FastCG::Random::NextFloat() * 2 - 1, FastCG::Random::NextFloat() * 2 - 1, FastCG::Random::NextFloat()};
            randomSample = glm::normalize(randomSample);
            auto scale = i / (float)N;
            scale = FastCG::MathF::Lerp(0.1f, 1.0f, FastCG::MathF::Pow(scale, 2));
            randomSample *= scale;
            randomSamples[i] = glm::vec4{randomSample, 0};
        }
    }

    const auto *CreateShadowMapTexture(const std::string &rName, uint32_t width, uint32_t height, FastCG::TextureFilter filter = FastCG::TextureFilter::LINEAR_FILTER, const void *pData = nullptr)
    {
        return FastCG::RenderingSystem::GetInstance()->CreateTexture({rName,
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

        mpShadowMapPassConstantsBuffer = RenderingSystem::GetInstance()->CreateBuffer({"Shadow Map Pass Constants",
                                                                                       BufferType::UNIFORM,
                                                                                       BufferUsage::DYNAMIC,
                                                                                       sizeof(ShadowMapPassConstants),
                                                                                       &mShadowMapPassConstants});

        {
            float data[] = {1};
            mpEmptyShadowMap = CreateShadowMapTexture("Empty Shadow Map", 1, 1, TextureFilter::POINT_FILTER, data);
        }

        mpShadowMapPassShader = RenderingSystem::GetInstance()->FindShader("ShadowMapPass");
        assert(mpShadowMapPassShader != nullptr);

        mSSAORenderTargets[0] = RenderingSystem::GetInstance()->CreateTexture({"Ambient Occlusion Map",
                                                                               mArgs.rScreenWidth,
                                                                               mArgs.rScreenHeight,
                                                                               TextureType::TEXTURE_2D,
                                                                               TextureFormat::R,
                                                                               {8},
                                                                               TextureDataType::FLOAT,
                                                                               TextureFilter::LINEAR_FILTER,
                                                                               TextureWrapMode::CLAMP,
                                                                               false});
        mSSAORenderTargets[1] = RenderingSystem::GetInstance()->CreateTexture({"Blurred Ambient Occlusion Map",
                                                                               mArgs.rScreenWidth,
                                                                               mArgs.rScreenHeight,
                                                                               TextureType::TEXTURE_2D,
                                                                               TextureFormat::R,
                                                                               {8},
                                                                               TextureDataType::FLOAT,
                                                                               TextureFilter::LINEAR_FILTER,
                                                                               TextureWrapMode::CLAMP,
                                                                               false});

        mpSSAOHighFrequencyPassConstantsBuffer = RenderingSystem::GetInstance()->CreateBuffer({"SSAO High Frequency Pass Constants",
                                                                                               BufferType::UNIFORM,
                                                                                               BufferUsage::DYNAMIC,
                                                                                               sizeof(SSAOHighFrequencyPassConstants),
                                                                                               &mSSAOHighFrequencyPassConstants});

        GenerateRandomSamplesInAHemisphere<NUMBER_OF_RANDOM_SAMPLES>(mSSAOHighFrequencyPassConstants.randomSamples);

        Random::SeedWithTime();
        auto pNoise = std::make_unique<glm::vec3[]>(NOISE_TEXTURE_SIZE);
        for (uint32_t i = 0; i < NOISE_TEXTURE_HEIGHT; i++)
        {
            for (uint32_t j = 0; j < NOISE_TEXTURE_WIDTH; j++)
            {
                glm::vec3 noise(Random::NextFloat() * 2 - 1, Random::NextFloat() * 2 - 1, 0);
                pNoise[i * NOISE_TEXTURE_WIDTH + j] = glm::normalize(noise);
            }
        }
        mpNoiseTexture = RenderingSystem::GetInstance()->CreateTexture({"Noise",
                                                                        NOISE_TEXTURE_WIDTH,
                                                                        NOISE_TEXTURE_HEIGHT,
                                                                        TextureType::TEXTURE_2D,
                                                                        TextureFormat::RGB,
                                                                        {8, 8, 8},
                                                                        TextureDataType::FLOAT,
                                                                        TextureFilter::POINT_FILTER,
                                                                        TextureWrapMode::REPEAT,
                                                                        false,
                                                                        &pNoise[0][0]});

        {
            glm::vec4 data[] = {glm::vec4{1, 1, 1, 1}};
            mpEmptySSAOTexture = RenderingSystem::GetInstance()->CreateTexture({"Empty SSAO",
                                                                                1,
                                                                                1,
                                                                                TextureType::TEXTURE_2D,
                                                                                TextureFormat::RGBA,
                                                                                {8, 8, 8, 8},
                                                                                TextureDataType::FLOAT,
                                                                                TextureFilter::POINT_FILTER,
                                                                                TextureWrapMode::REPEAT,
                                                                                false,
                                                                                data});
        }

        mpSSAOHighFrequencyPassShader = RenderingSystem::GetInstance()->FindShader("SSAOHighFrequencyPass");
        assert(mpSSAOHighFrequencyPassShader != nullptr);
        mpSSAOBlurPassShader = RenderingSystem::GetInstance()->FindShader("SSAOBlurPass");
        assert(mpSSAOBlurPassShader != nullptr);

        mpQuadMesh = StandardGeometries::CreateXYPlane("Fullscreen Quad", 2, 2, 1, 1, glm::vec3(0, 0, 0));
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Finalize()
    {
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

        mpShadowMapPassShader = nullptr;

        if (mpEmptyShadowMap != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyTexture(mpEmptyShadowMap);
            mpEmptyShadowMap = nullptr;
        }

        if (mpShadowMapPassConstantsBuffer != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyBuffer(mpShadowMapPassConstantsBuffer);
            mpShadowMapPassConstantsBuffer = nullptr;
        }

        mpSSAOHighFrequencyPassShader = nullptr;
        mpSSAOBlurPassShader = nullptr;

        if (mpQuadMesh != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyMesh(mpQuadMesh);
        }

        if (mpEmptySSAOTexture != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyTexture(mpEmptySSAOTexture);
        }

        if (mpNoiseTexture != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyTexture(mpNoiseTexture);
        }

        if (mpSSAOHighFrequencyPassConstantsBuffer != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyBuffer(mpSSAOHighFrequencyPassConstantsBuffer);
            mpSSAOHighFrequencyPassConstantsBuffer = nullptr;
        }

        for (const auto *pRenderTarget : mSSAORenderTargets)
        {
            RenderingSystem::GetInstance()->DestroyTexture(pRenderTarget);
        }
        memset(mSSAORenderTargets.data(), 0, sizeof(mSSAORenderTargets));
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const typename BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::ShadowMap &BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetOrCreateShadowMap(const Light *pLight)
    {
        auto shadowMapKey = GetShadowMapKey(pLight);
        auto it = mShadowMaps.find(shadowMapKey);
        if (it == mShadowMaps.end())
        {
            it = mShadowMaps.emplace(shadowMapKey, ShadowMap{pLight, CreateShadowMapTexture("Shadow Map", mArgs.rScreenWidth, mArgs.rScreenHeight)}).first;
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

                    auto modelViewProjection = rShadowMap.GetProjection() * rShadowMap.GetView() * model;
                    UpdateShadowMapPassConstants(modelViewProjection, pRenderingContext);
                    pRenderingContext->Bind(mpShadowMapPassConstantsBuffer, Shader::SHADOW_MAP_PASS_CONSTANTS_BINDING_INDEX);

                    pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, pMesh->GetIndexCount(), 0, 0);

                    mArgs.rRenderingStatistics.drawCalls++;
                }
            }
        }
        pRenderingContext->PopDebugMarker();
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GenerateAmbientOcculusionMap(const glm::mat4 &rProjection, float fov, const Texture *pDepth, RenderingContext *pRenderingContext)
    {
        pRenderingContext->PushDebugMarker("Ambient Occlusion Map Generation");
        {
            pRenderingContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
            pRenderingContext->SetDepthTest(false);
            pRenderingContext->SetDepthWrite(false);
            pRenderingContext->SetStencilTest(false);
            pRenderingContext->SetScissorTest(false);
            pRenderingContext->SetCullMode(Face::NONE);
            pRenderingContext->SetBlend(false);

            pRenderingContext->PushDebugMarker("SSAO High Frequency Pass");
            {
                pRenderingContext->SetRenderTargets(&mSSAORenderTargets[0], 1);
                pRenderingContext->ClearRenderTarget(0, Colors::NONE);

                pRenderingContext->Bind(mpSSAOHighFrequencyPassShader);

                pRenderingContext->Bind(mpSceneConstantsBuffer, Shader::SCENE_CONSTANTS_BINDING_INDEX);
                UpdateSSAOHighFrequencyPassConstants(rProjection, fov, pDepth, pRenderingContext);
                pRenderingContext->Bind(mpSSAOHighFrequencyPassConstantsBuffer, Shader::SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_BINDING_INDEX);

                pRenderingContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(), mpQuadMesh->GetVertexBufferCount());
                pRenderingContext->SetIndexBuffer(mpQuadMesh->GetIndexBuffer());

                pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, mpQuadMesh->GetIndexCount(), 0, 0);

                mArgs.rRenderingStatistics.drawCalls++;
            }
            pRenderingContext->PopDebugMarker();

            if (mSSAOBlurEnabled)
            {
                pRenderingContext->PushDebugMarker("SSAO Blur Pass");
                {
                    pRenderingContext->SetRenderTargets(&mSSAORenderTargets[1], 1);
                    pRenderingContext->ClearRenderTarget(0, Colors::NONE);

                    pRenderingContext->Bind(mpSSAOBlurPassShader);

                    pRenderingContext->Bind(mSSAORenderTargets[0], "uAmbientOcclusion", 0);

                    pRenderingContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(), mpQuadMesh->GetVertexBufferCount());
                    pRenderingContext->SetIndexBuffer(mpQuadMesh->GetIndexBuffer());

                    pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, mpQuadMesh->GetIndexCount(), 0, 0);

                    mArgs.rRenderingStatistics.drawCalls++;
                }
                pRenderingContext->PopDebugMarker();
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
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::BindSSAOTexture(bool isSSAOEnabled, RenderingContext *pRenderingContext) const
    {
        if (isSSAOEnabled)
        {
            if (mSSAOBlurEnabled)
            {
                pRenderingContext->Bind(mSSAORenderTargets[1], "uAmbientOcclusionMap", 3);
            }
            else
            {
                pRenderingContext->Bind(mSSAORenderTargets[0], "uAmbientOcclusionMap", 3);
            }
        }
        else
        {
            pRenderingContext->Bind(mpEmptySSAOTexture, "uAmbientOcclusionMap", 3);
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstants(const PointLight *pPointLight, const glm::mat4 &rView, bool isSSAOEnabled, RenderingContext *pRenderingContext)
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

        BindSSAOTexture(isSSAOEnabled, pRenderingContext);

        pRenderingContext->Copy(mpLightingConstantsBuffer, sizeof(LightingConstants), &mLightingConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstants(const DirectionalLight *pDirectionalLight, const glm::vec3 &rDirection, bool isSSAOEnabled, RenderingContext *pRenderingContext)
    {
        mLightingConstants.light0Position = glm::vec4(rDirection, -1);
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

        BindSSAOTexture(isSSAOEnabled, pRenderingContext);

        pRenderingContext->Copy(mpLightingConstantsBuffer, sizeof(LightingConstants), &mLightingConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateSceneConstants(const glm::mat4 &rView, const glm::mat4 &rProjection, RenderingContext *pRenderingContext)
    {
        mSceneConstants.view = rView;
        mSceneConstants.inverseView = glm::inverse(rView);
        mSceneConstants.projection = rProjection;
        mSceneConstants.inverseProjection = glm::inverse(rProjection);
        mSceneConstants.screenSize = glm::vec2{mArgs.rScreenWidth, mArgs.rScreenHeight};
        mSceneConstants.pointSize = 1.0f; // TODO:

        pRenderingContext->Copy(mpSceneConstantsBuffer, sizeof(SceneConstants), &mSceneConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateShadowMapPassConstants(const glm::mat4 &rModelViewProjection, RenderingContext *pRenderingContext)
    {
        mShadowMapPassConstants.modelViewProjection = rModelViewProjection;

        pRenderingContext->Copy(mpShadowMapPassConstantsBuffer, sizeof(ShadowMapPassConstants), &mShadowMapPassConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateSSAOHighFrequencyPassConstants(const glm::mat4 &rProjection, float fov, const Texture *pDepth, RenderingContext *pRenderingContext)
    {
        mSSAOHighFrequencyPassConstants.projection = rProjection;
        mSSAOHighFrequencyPassConstants.aspectRatio = mArgs.rScreenWidth / (float)mArgs.rScreenHeight;
        mSSAOHighFrequencyPassConstants.tanHalfFov = MathF::Tan((MathF::DEGREES_TO_RADIANS * fov) / 2.0f);

        pRenderingContext->Copy(mpSSAOHighFrequencyPassConstantsBuffer, sizeof(SSAOHighFrequencyPassConstants), &mSSAOHighFrequencyPassConstants);

        pRenderingContext->Bind(mpNoiseTexture, "uNoise", 0);
        pRenderingContext->Bind(pDepth, "uDepth", 1);
    }

}