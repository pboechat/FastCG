#include <FastCG/World/WorldSystem.h>
#include <FastCG/Rendering/StandardGeometries.h>
#include <FastCG/Debug/DebugMenuSystem.h>
#include <FastCG/Core/Random.h>
#include <FastCG/Core/Math.h>

#include <imgui.h>

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

    const auto *CreateShadowMapTexture(const std::string &rName, uint32_t width, uint32_t height, const void *pData = nullptr)
    {
        return FastCG::GraphicsSystem::GetInstance()->CreateTexture({rName,
                                                                     width,
                                                                     height,
                                                                     FastCG::TextureType::TEXTURE_2D,
                                                                     FastCG::TextureFormat::DEPTH,
                                                                     {32},
                                                                     FastCG::TextureDataType::FLOAT,
                                                                     FastCG::TextureFilter::POINT_FILTER,
                                                                     FastCG::TextureWrapMode::CLAMP,
                                                                     false,
                                                                     pData});
    }

}

namespace FastCG
{
    const uint32_t SHADOW_MAP_SIZE = 4096;
    const glm::mat4 INVALID_SHADOW_MAP_PROJECTION = glm::ortho(-FLT_MAX, FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX, FLT_MAX);

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    typename BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::ShadowMapKey BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetShadowMapKey(const Light *pLight) const
    {
        return (ShadowMapKey)pLight;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Initialize()
    {
#ifdef _DEBUG
        DebugMenuSystem::GetInstance()->AddItem("World Renderer", std::bind(&BaseWorldRenderer::DebugMenuItemCallback, this, std::placeholders::_1));
#endif

        mpLightingConstantsBuffer = GraphicsSystem::GetInstance()->CreateBuffer({"Lighting Constants",
                                                                                 BufferType::UNIFORM,
                                                                                 BufferUsage::DYNAMIC,
                                                                                 sizeof(LightingConstants),
                                                                                 &mLightingConstants});
        mpInstanceConstantsBuffer = GraphicsSystem::GetInstance()->CreateBuffer({"Instance Constants",
                                                                                 BufferType::SHADER_STORAGE,
                                                                                 BufferUsage::DYNAMIC,
                                                                                 sizeof(InstanceConstants),
                                                                                 &mInstanceConstants});
        mpSceneConstantsBuffer = GraphicsSystem::GetInstance()->CreateBuffer({"Scene Constants",
                                                                              BufferType::UNIFORM,
                                                                              BufferUsage::DYNAMIC,
                                                                              sizeof(SceneConstants),
                                                                              &mSceneConstants});

        mpShadowMapPassConstantsBuffer = GraphicsSystem::GetInstance()->CreateBuffer({"Shadow Map Pass Constants",
                                                                                      BufferType::SHADER_STORAGE,
                                                                                      BufferUsage::DYNAMIC,
                                                                                      sizeof(ShadowMapPassConstants),
                                                                                      &mShadowMapPassConstants});

        {
            float data[] = {1};
            mpEmptyShadowMap = CreateShadowMapTexture("Empty Shadow Map", 1, 1, data);
        }

        mpShadowMapPassShader = GraphicsSystem::GetInstance()->FindShader("ShadowMapPass");
        assert(mpShadowMapPassShader != nullptr);

        CreateSSAORenderTargets();

        mpSSAOHighFrequencyPassConstantsBuffer = GraphicsSystem::GetInstance()->CreateBuffer({"SSAO High Frequency Pass Constants",
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
        mpNoiseTexture = GraphicsSystem::GetInstance()->CreateTexture({"Noise",
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
            mpEmptySSAOTexture = GraphicsSystem::GetInstance()->CreateTexture({"Empty SSAO",
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

        mpSSAOHighFrequencyPassShader = GraphicsSystem::GetInstance()->FindShader("SSAOHighFrequencyPass");
        assert(mpSSAOHighFrequencyPassShader != nullptr);
        mpSSAOBlurPassShader = GraphicsSystem::GetInstance()->FindShader("SSAOBlurPass");
        assert(mpSSAOBlurPassShader != nullptr);

        mpTonemapShader = GraphicsSystem::GetInstance()->FindShader("TonemapPass");
        assert(mpTonemapShader != nullptr);

        mpQuadMesh = StandardGeometries::CreateXYPlane("Fullscreen Quad", 2, 2, 1, 1, glm::vec3(0, 0, 0));
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::SetGraphicsContextState(const GraphicsContextState &rGraphicsContextState, GraphicsContext *pGraphicsContext) const
    {
        pGraphicsContext->SetDepthTest(rGraphicsContextState.depthTest);
        pGraphicsContext->SetDepthWrite(rGraphicsContextState.depthWrite);
        pGraphicsContext->SetDepthFunc(rGraphicsContextState.depthFunc);
        pGraphicsContext->SetScissorTest(rGraphicsContextState.scissorTest);
        if (rGraphicsContextState.stencilTest)
        {
            pGraphicsContext->SetStencilTest(true);
            pGraphicsContext->SetStencilFunc(Face::BACK, rGraphicsContextState.stencilBackState.compareOp, rGraphicsContextState.stencilBackState.reference, rGraphicsContextState.stencilBackState.compareMask);
            pGraphicsContext->SetStencilOp(Face::BACK, rGraphicsContextState.stencilBackState.stencilFailOp, rGraphicsContextState.stencilBackState.depthFailOp, rGraphicsContextState.stencilBackState.passOp);
            pGraphicsContext->SetStencilWriteMask(Face::BACK, rGraphicsContextState.stencilBackState.writeMask);
            pGraphicsContext->SetStencilFunc(Face::FRONT, rGraphicsContextState.stencilFrontState.compareOp, rGraphicsContextState.stencilFrontState.reference, rGraphicsContextState.stencilFrontState.compareMask);
            pGraphicsContext->SetStencilOp(Face::FRONT, rGraphicsContextState.stencilFrontState.stencilFailOp, rGraphicsContextState.stencilFrontState.depthFailOp, rGraphicsContextState.stencilFrontState.passOp);
            pGraphicsContext->SetStencilWriteMask(Face::FRONT, rGraphicsContextState.stencilFrontState.writeMask);
        }
        else
        {
            pGraphicsContext->SetStencilTest(false);
        }
        pGraphicsContext->SetCullMode(rGraphicsContextState.cullMode);
        if (rGraphicsContextState.blend)
        {
            pGraphicsContext->SetBlend(true);
            pGraphicsContext->SetBlendFunc(rGraphicsContextState.blendState.colorOp, rGraphicsContextState.blendState.alphaOp);
            pGraphicsContext->SetBlendFactors(rGraphicsContextState.blendState.srcColorFactor, rGraphicsContextState.blendState.dstColorFactor, rGraphicsContextState.blendState.srcAlphaFactor, rGraphicsContextState.blendState.dstAlphaFactor);
        }
        else
        {
            pGraphicsContext->SetBlend(false);
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::CreateSSAORenderTargets()
    {
        mSSAORenderTargets[0] = GraphicsSystem::GetInstance()->CreateTexture({"Ambient Occlusion Map",
                                                                              mArgs.rScreenWidth,
                                                                              mArgs.rScreenHeight,
                                                                              TextureType::TEXTURE_2D,
                                                                              TextureFormat::R,
                                                                              {8},
                                                                              TextureDataType::FLOAT,
                                                                              TextureFilter::LINEAR_FILTER,
                                                                              TextureWrapMode::CLAMP,
                                                                              false});
        mSSAORenderTargets[1] = GraphicsSystem::GetInstance()->CreateTexture({"Blurred Ambient Occlusion Map",
                                                                              mArgs.rScreenWidth,
                                                                              mArgs.rScreenHeight,
                                                                              TextureType::TEXTURE_2D,
                                                                              TextureFormat::R,
                                                                              {8},
                                                                              TextureDataType::FLOAT,
                                                                              TextureFilter::LINEAR_FILTER,
                                                                              TextureWrapMode::CLAMP,
                                                                              false});
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::DestroySSAORenderTargets()
    {
        for (const auto *pRenderTarget : mSSAORenderTargets)
        {
            if (pRenderTarget != nullptr)
            {
                GraphicsSystem::GetInstance()->DestroyTexture(pRenderTarget);
            }
        }
        mSSAORenderTargets = {};
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Resize()
    {
        if (GraphicsSystem::GetInstance()->IsInitialized())
        {
            DestroySSAORenderTargets();
            CreateSSAORenderTargets();
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Finalize()
    {
        if (mpSceneConstantsBuffer != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(mpSceneConstantsBuffer);
            mpSceneConstantsBuffer = nullptr;
        }

        if (mpInstanceConstantsBuffer != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(mpInstanceConstantsBuffer);
            mpInstanceConstantsBuffer = nullptr;
        }

        if (mpLightingConstantsBuffer != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(mpLightingConstantsBuffer);
            mpLightingConstantsBuffer = nullptr;
        }

        mpShadowMapPassShader = nullptr;

        if (mpEmptyShadowMap != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyTexture(mpEmptyShadowMap);
            mpEmptyShadowMap = nullptr;
        }

        if (mpShadowMapPassConstantsBuffer != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(mpShadowMapPassConstantsBuffer);
            mpShadowMapPassConstantsBuffer = nullptr;
        }

        mpSSAOHighFrequencyPassShader = nullptr;
        mpSSAOBlurPassShader = nullptr;

        mpQuadMesh = nullptr;

        if (mpEmptySSAOTexture != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyTexture(mpEmptySSAOTexture);
        }

        if (mpNoiseTexture != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyTexture(mpNoiseTexture);
        }

        if (mpSSAOHighFrequencyPassConstantsBuffer != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(mpSSAOHighFrequencyPassConstantsBuffer);
            mpSSAOHighFrequencyPassConstantsBuffer = nullptr;
        }

        mpTonemapShader = nullptr;
    }

#ifdef _DEBUG
    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::DebugMenuItemCallback(int &result)
    {
        if (ImGui::BeginMenu("PCSS"))
        {
            ImGui::SliderFloat("UV Scale", &mLightingConstants.pcssData.uvScale, 0, 0.2f);
            ImGui::SliderFloat("Shadow Map Bias", &mLightingConstants.pcssData.shadowMapData.bias, 0, 0.1f);
            ImGui::SliderInt("Blocker Search Samples", &mLightingConstants.pcssData.blockerSearchSamples, 1, 64);
            ImGui::SliderInt("PCF Samples", &mLightingConstants.pcssData.pcfSamples, 1, 64);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("SSAO"))
        {
            ImGui::Checkbox("Blur enabled", &mSSAOBlurEnabled);
            ImGui::SliderFloat("Radius", &mSSAOHighFrequencyPassConstants.radius, 0.01f, 0.1f);
            ImGui::SliderFloat("Distance scale", &mSSAOHighFrequencyPassConstants.distanceScale, 1, 100);
            ImGui::EndMenu();
        }
    }
#endif

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const typename BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::ShadowMap &BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetOrCreateShadowMap(const Light *pLight)
    {
        auto shadowMapKey = GetShadowMapKey(pLight);
        auto it = mShadowMaps.find(shadowMapKey);
        if (it == mShadowMaps.end())
        {
            it = mShadowMaps.emplace(shadowMapKey, ShadowMap{pLight, CreateShadowMapTexture("Shadow Map", SHADOW_MAP_SIZE, SHADOW_MAP_SIZE)}).first;
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
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GenerateShadowMaps(GraphicsContext *pGraphicsContext)
    {
        auto &rShadowCastersRenderBatch = mArgs.rRenderBatches[0];
        assert(rShadowCastersRenderBatch.type == RenderBatchType::SHADOW_CASTERS);
        if (rShadowCastersRenderBatch.renderablesPerMesh.empty())
        {
            return;
        }

        pGraphicsContext->PushDebugMarker("Shadow Casters Passes");
        {
            pGraphicsContext->SetViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
            pGraphicsContext->SetDepthTest(true);
            pGraphicsContext->SetDepthWrite(true);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetScissorTest(false);
            pGraphicsContext->SetCullMode(Face::BACK);
            pGraphicsContext->SetBlend(false);

            pGraphicsContext->BindShader(mpShadowMapPassShader);

            const auto &rDirectionalLights = WorldSystem::GetInstance()->GetDirectionalLights();
            for (size_t i = 0; i < rDirectionalLights.size(); ++i)
            {
                const auto *pDirectionalLight = rDirectionalLights[i];

                const auto &rShadowMap = GetOrCreateShadowMap(pDirectionalLight);

                const auto *pShadowMapTexture = rShadowMap.GetTexture();

                for (auto it = rShadowCastersRenderBatch.renderablesPerMesh.cbegin(); it != rShadowCastersRenderBatch.renderablesPerMesh.cend(); ++it)
                {
                    const auto &pMesh = it->first;
                    const auto &rRenderables = it->second;

                    pGraphicsContext->SetVertexBuffers(pMesh->GetVertexBuffers(), pMesh->GetVertexBufferCount());
                    pGraphicsContext->SetIndexBuffer(pMesh->GetIndexBuffer());

                    pGraphicsContext->SetRenderTargets(&pShadowMapTexture, 1);
                    pGraphicsContext->ClearDepthTarget(0, 1);

                    auto instanceCount = UpdateShadowMapPassConstants(rRenderables, rShadowMap.GetView(), rShadowMap.GetProjection(), pGraphicsContext);
                    pGraphicsContext->BindResource(mpShadowMapPassConstantsBuffer, SHADOW_MAP_PASS_CONSTANTS_SHADER_RESOURCE_INDEX);

                    if (instanceCount == 1)
                    {
                        pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, pMesh->GetIndexCount(), 0);
                    }
                    else
                    {
                        pGraphicsContext->DrawInstancedIndexed(PrimitiveType::TRIANGLES, 0, instanceCount, 0, pMesh->GetIndexCount(), 0);
                    }

                    mArgs.rRenderingStatistics.drawCalls++;
                }
            }
        }
        pGraphicsContext->PopDebugMarker();
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GenerateAmbientOcculusionMap(const glm::mat4 &rProjection, float fov, const Texture *pDepth, GraphicsContext *pGraphicsContext)
    {
        pGraphicsContext->PushDebugMarker("Ambient Occlusion Map Generation");
        {
            pGraphicsContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
            pGraphicsContext->SetDepthTest(false);
            pGraphicsContext->SetDepthWrite(false);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetScissorTest(false);
            pGraphicsContext->SetCullMode(Face::NONE);
            pGraphicsContext->SetBlend(false);

            pGraphicsContext->PushDebugMarker("SSAO High Frequency Pass");
            {
                pGraphicsContext->SetRenderTargets(&mSSAORenderTargets[0], 1);
                pGraphicsContext->ClearRenderTarget(0, Colors::NONE);

                pGraphicsContext->BindShader(mpSSAOHighFrequencyPassShader);

                pGraphicsContext->BindResource(mpSceneConstantsBuffer, SCENE_CONSTANTS_SHADER_RESOURCE_INDEX);

                UpdateSSAOHighFrequencyPassConstants(rProjection, fov, pDepth, pGraphicsContext);
                pGraphicsContext->BindResource(mpSSAOHighFrequencyPassConstantsBuffer, SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_SHADER_RESOURCE_INDEX);

                pGraphicsContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(), mpQuadMesh->GetVertexBufferCount());
                pGraphicsContext->SetIndexBuffer(mpQuadMesh->GetIndexBuffer());

                pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpQuadMesh->GetIndexCount(), 0);

                mArgs.rRenderingStatistics.drawCalls++;
            }
            pGraphicsContext->PopDebugMarker();

            if (mSSAOBlurEnabled)
            {
                pGraphicsContext->PushDebugMarker("SSAO Blur Pass");
                {
                    pGraphicsContext->SetRenderTargets(&mSSAORenderTargets[1], 1);
                    pGraphicsContext->ClearRenderTarget(0, Colors::NONE);

                    pGraphicsContext->BindShader(mpSSAOBlurPassShader);

                    pGraphicsContext->BindResource(mSSAORenderTargets[0], "uAmbientOcclusion", 0);

                    pGraphicsContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(), mpQuadMesh->GetVertexBufferCount());
                    pGraphicsContext->SetIndexBuffer(mpQuadMesh->GetIndexBuffer());

                    pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpQuadMesh->GetIndexCount(), 0);

                    mArgs.rRenderingStatistics.drawCalls++;
                }
                pGraphicsContext->PopDebugMarker();
            }
        }
        pGraphicsContext->PopDebugMarker();
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Tonemap(const Texture *pSourceRenderTarget, const Texture *pDestinationRenderTarget, GraphicsContext *pGraphicsContext)
    {
        pGraphicsContext->PushDebugMarker("Tonemapping");
        {
            pGraphicsContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
            pGraphicsContext->SetDepthTest(false);
            pGraphicsContext->SetDepthWrite(false);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetScissorTest(false);
            pGraphicsContext->SetCullMode(Face::NONE);
            pGraphicsContext->SetBlend(false);

            pGraphicsContext->SetRenderTargets(&pDestinationRenderTarget, 1);

            pGraphicsContext->BindShader(mpTonemapShader);

            pGraphicsContext->BindResource(pSourceRenderTarget, "uSource", 0);

            pGraphicsContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(), mpQuadMesh->GetVertexBufferCount());
            pGraphicsContext->SetIndexBuffer(mpQuadMesh->GetIndexBuffer());

            pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpQuadMesh->GetIndexCount(), 0);

            mArgs.rRenderingStatistics.drawCalls++;
        }
        pGraphicsContext->PopDebugMarker();
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::BindMaterial(const Material *pMaterial, GraphicsContext *pGraphicsContext)
    {
        SetGraphicsContextState(pMaterial->GetGraphicsContextState(), pGraphicsContext);

        const auto *pConstantBuffer = pMaterial->GetConstantBuffer();
        if (pConstantBuffer != nullptr)
        {
            pGraphicsContext->Copy(pMaterial->GetConstantBuffer(), pMaterial->GetConstantBufferSize(), pMaterial->GetConstantBufferData());
        }

        pGraphicsContext->BindShader(pMaterial->GetShader());

        if (pConstantBuffer != nullptr)
        {
            pGraphicsContext->BindResource(pMaterial->GetConstantBuffer(), MATERIAL_CONSTANTS_SHADER_RESOURCE_INDEX);
        }

        for (size_t i = 0; i < pMaterial->GetTextureCount(); ++i)
        {
            std::string name;
            const Texture *pTexture;
            pMaterial->GetTextureAt(i, name, pTexture);
            if (pTexture != nullptr)
            {
                pGraphicsContext->BindResource(pTexture, name.c_str(), (uint32_t)i);
            }
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateInstanceConstants(const glm::mat4 &rModel, const glm::mat4 &rView, const glm::mat4 &rProjection, GraphicsContext *pGraphicsContext)
    {
        auto &rInstanceData = mInstanceConstants.instanceData[0];

        auto modelView = rView * rModel;

        rInstanceData.model = rModel;
        rInstanceData.modelView = modelView;
        rInstanceData.modelViewInverseTranspose = glm::transpose(glm::inverse(modelView));
        rInstanceData.modelViewProjection = rProjection * modelView;

        pGraphicsContext->Copy(mpInstanceConstantsBuffer, sizeof(InstanceConstants::instanceData[0]), &mInstanceConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    uint32_t BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateInstanceConstants(const std::vector<const Renderable *> &rRenderables, const glm::mat4 &rView, const glm::mat4 &rProjection, GraphicsContext *pGraphicsContext)
    {
        uint32_t instanceCount = 0;
        for (const auto *pRenderable : rRenderables)
        {
            if (!pRenderable->GetGameObject()->IsActive())
            {
                continue;
            }

            auto &rInstanceData = mInstanceConstants.instanceData[instanceCount++];

            auto model = pRenderable->GetGameObject()->GetTransform()->GetModel();
            auto modelView = rView * model;

            rInstanceData.model = model;
            rInstanceData.modelView = modelView;
            rInstanceData.modelViewInverseTranspose = glm::transpose(glm::inverse(modelView));
            rInstanceData.modelViewProjection = rProjection * modelView;
        }

        assert(instanceCount < MAX_NUM_INSTANCES);

        if (instanceCount > 0)
        {
            pGraphicsContext->Copy(mpInstanceConstantsBuffer, sizeof(InstanceConstants::instanceData[0]) * instanceCount, &mInstanceConstants);
        }

        return instanceCount;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdatePCSSConstants(const PointLight *pPointLight, float nearClip, GraphicsContext *pGraphicsContext)
    {
        // TODO:

        mLightingConstants.pcssData.shadowMapData.viewProjection = INVALID_SHADOW_MAP_PROJECTION;
        mLightingConstants.pcssData.nearClip = 0;

        pGraphicsContext->BindResource(mpEmptyShadowMap, "uShadowMap", 2);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdatePCSSConstants(const DirectionalLight *pDirectionalLight, float nearClip, GraphicsContext *pGraphicsContext)
    {
        ShadowMap shadowMap;
        if (GetShadowMap(pDirectionalLight, shadowMap))
        {
            auto view = shadowMap.GetView();
            mLightingConstants.pcssData.shadowMapData.viewProjection = shadowMap.GetProjection() * view;
            mLightingConstants.pcssData.nearClip = nearClip;

            pGraphicsContext->BindResource(shadowMap.GetTexture(), "uShadowMap", 2);
        }
        else
        {
            mLightingConstants.pcssData.shadowMapData.viewProjection = INVALID_SHADOW_MAP_PROJECTION;
            mLightingConstants.pcssData.nearClip = 0;

            pGraphicsContext->BindResource(mpEmptyShadowMap, "uShadowMap", 2);
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateSSAOConstants(bool isSSAOEnabled, GraphicsContext *pGraphicsContext) const
    {
        if (isSSAOEnabled)
        {
            if (mSSAOBlurEnabled)
            {
                pGraphicsContext->BindResource(mSSAORenderTargets[1], "uAmbientOcclusionMap", 3);
            }
            else
            {
                pGraphicsContext->BindResource(mSSAORenderTargets[0], "uAmbientOcclusionMap", 3);
            }
        }
        else
        {
            pGraphicsContext->BindResource(mpEmptySSAOTexture, "uAmbientOcclusionMap", 3);
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstants(const PointLight *pPointLight, const glm::mat4 &rInverseView, float nearClip, bool isSSAOEnabled, GraphicsContext *pGraphicsContext)
    {
        const auto &rLightPosition = pPointLight->GetGameObject()->GetTransform()->GetWorldPosition();
        mLightingConstants.light0Position = glm::vec4(rLightPosition, 1);
        mLightingConstants.light0ViewPosition = rInverseView * mLightingConstants.light0Position;
        mLightingConstants.light0DiffuseColor = pPointLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pPointLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pPointLight->GetIntensity();
        mLightingConstants.light0ConstantAttenuation = pPointLight->GetConstantAttenuation();
        mLightingConstants.light0LinearAttenuation = pPointLight->GetLinearAttenuation();
        mLightingConstants.light0QuadraticAttenuation = pPointLight->GetQuadraticAttenuation();
        mLightingConstants.ambientColor = mArgs.rAmbientLight;

        UpdatePCSSConstants(pPointLight, nearClip, pGraphicsContext);
        UpdateSSAOConstants(isSSAOEnabled, pGraphicsContext);

        pGraphicsContext->Copy(mpLightingConstantsBuffer, sizeof(LightingConstants), &mLightingConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstants(const DirectionalLight *pDirectionalLight, const glm::vec3 &rViewDirection, float nearClip, bool isSSAOEnabled, GraphicsContext *pGraphicsContext)
    {
        mLightingConstants.light0Position = glm::vec4(rViewDirection, -1);
        mLightingConstants.light0ViewPosition = mLightingConstants.light0Position;
        mLightingConstants.light0DiffuseColor = pDirectionalLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pDirectionalLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pDirectionalLight->GetIntensity();
        mLightingConstants.ambientColor = mArgs.rAmbientLight;

        UpdatePCSSConstants(pDirectionalLight, nearClip, pGraphicsContext);
        UpdateSSAOConstants(isSSAOEnabled, pGraphicsContext);

        pGraphicsContext->Copy(mpLightingConstantsBuffer, sizeof(LightingConstants), &mLightingConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateSceneConstants(const glm::mat4 &rView, const glm::mat4 &rInverseView, const glm::mat4 &rProjection, GraphicsContext *pGraphicsContext)
    {
        mSceneConstants.view = rView;
        mSceneConstants.inverseView = rInverseView;
        mSceneConstants.projection = rProjection;
        mSceneConstants.inverseProjection = glm::inverse(rProjection);
        mSceneConstants.screenSize = glm::vec2{mArgs.rScreenWidth, mArgs.rScreenHeight};
        mSceneConstants.pointSize = 1.0f; // TODO:

        pGraphicsContext->Copy(mpSceneConstantsBuffer, sizeof(SceneConstants), &mSceneConstants);
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    uint32_t BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateShadowMapPassConstants(const std::vector<const Renderable *> &rRenderables, const glm::mat4 &rView, const glm::mat4 &rProjection, GraphicsContext *pGraphicsContext)
    {
        uint32_t instanceCount = 0;
        for (const auto *pRenderable : rRenderables)
        {
            if (!pRenderable->GetGameObject()->IsActive())
            {
                continue;
            }

            auto &rInstanceData = mShadowMapPassConstants.instanceData[instanceCount++];

            rInstanceData.modelViewProjection = rProjection * rView * pRenderable->GetGameObject()->GetTransform()->GetModel();
        }

        assert(instanceCount < MAX_NUM_INSTANCES);

        if (instanceCount > 0)
        {
            pGraphicsContext->Copy(mpShadowMapPassConstantsBuffer, sizeof(ShadowMapPassConstants::instanceData[0]) * instanceCount, &mShadowMapPassConstants);
        }

        return instanceCount;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateSSAOHighFrequencyPassConstants(const glm::mat4 &rProjection, float fov, const Texture *pDepth, GraphicsContext *pGraphicsContext)
    {
        mSSAOHighFrequencyPassConstants.projection = rProjection;
        mSSAOHighFrequencyPassConstants.aspectRatio = mArgs.rScreenWidth / (float)mArgs.rScreenHeight;
        mSSAOHighFrequencyPassConstants.tanHalfFov = MathF::Tan((MathF::DEGREES_TO_RADIANS * fov) / 2.0f);

        pGraphicsContext->Copy(mpSSAOHighFrequencyPassConstantsBuffer, sizeof(SSAOHighFrequencyPassConstants), &mSSAOHighFrequencyPassConstants);

        pGraphicsContext->BindResource(mpNoiseTexture, "uNoise", 0);
        pGraphicsContext->BindResource(pDepth, "uDepth", 1);
    }

}