#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Math.h>
#include <FastCG/Core/Random.h>
#include <FastCG/Debug/DebugMenuSystem.h>
#include <FastCG/Rendering/StandardGeometries.h>
#include <FastCG/World/WorldSystem.h>

#include <imgui.h>

namespace
{
    template <size_t N>
    void GenerateRandomSamplesInAHemisphere(glm::vec4 randomSamples[N])
    {
        for (uint32_t i = 0; i < N; i++)
        {
            glm::vec3 randomSample{FastCG::Random::NextFloat() * 2 - 1, FastCG::Random::NextFloat() * 2 - 1,
                                   FastCG::Random::NextFloat()};
            randomSample = glm::normalize(randomSample);
            auto scale = i / (float)N;
            scale = FastCG::MathF::Lerp(0.1f, 1.0f, FastCG::MathF::Pow(scale, 2));
            randomSample *= scale;
            randomSamples[i] = glm::vec4{randomSample, 0};
        }
    }

    const auto *CreateShadowMapTexture(const std::string &rName, uint32_t width, uint32_t height,
                                       const uint8_t *pData = nullptr)
    {
        return FastCG::GraphicsSystem::GetInstance()->CreateTexture(
            {rName, width, height, 1, 1, FastCG::TextureType::TEXTURE_2D,
             FastCG::TextureUsageFlagBit::SAMPLED | FastCG::TextureUsageFlagBit::RENDER_TARGET,
             FastCG::TextureFormat::X8_D24_UNORM_PACK32, FastCG::TextureFilter::POINT_FILTER,
             FastCG::TextureWrapMode::CLAMP, pData});
    }

}

namespace FastCG
{
    const uint32_t SHADOW_MAP_SIZE = 4096;
    const glm::mat4 INVALID_SHADOW_MAP_PROJECTION = glm::ortho(-FLT_MAX, FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX, FLT_MAX);

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    typename BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::ShadowMapKey BaseWorldRenderer<
        InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetShadowMapKey(const Light *pLight) const
    {
        return (ShadowMapKey)pLight;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Initialize()
    {
#if _DEBUG
        DebugMenuSystem::GetInstance()->AddItem(
            "World Renderer", std::bind(&BaseWorldRenderer::DebugMenuItemCallback, this, std::placeholders::_1));
#endif

        {
            const uint8_t data[] = {255, 255, 255, 255};
            mpEmptyShadowMap = CreateShadowMapTexture("Empty Shadow Map", 1, 1, data);
        }

        mpShadowMapPassShader = GraphicsSystem::GetInstance()->FindShader("ShadowMapPass");
        assert(mpShadowMapPassShader != nullptr);

        CreateSSAORenderTargets();

        mpSSAOHighFrequencyPassConstantsBuffer = GraphicsSystem::GetInstance()->CreateBuffer(
            {"SSAO High Frequency Pass Constants", BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC,
             sizeof(SSAOHighFrequencyPassConstants), &mSSAOHighFrequencyPassConstants});

        GenerateRandomSamplesInAHemisphere<NUMBER_OF_RANDOM_SAMPLES>(mSSAOHighFrequencyPassConstants.randomSamples);

        {
            auto pData = std::make_unique<uint8_t[]>(NOISE_TEXTURE_WIDTH * NOISE_TEXTURE_HEIGHT * 4);
            auto *pNoise = pData.get();
            for (uint32_t i = 0; i < NOISE_TEXTURE_HEIGHT; i++)
            {
                for (uint32_t j = 0; j < NOISE_TEXTURE_WIDTH; j++, pNoise += 4)
                {
                    auto noise =
                        glm::normalize(glm::vec4(Random::NextFloat() * 2 - 1, Random::NextFloat() * 2 - 1, 0, 0));
                    pNoise[0] = (uint8_t)(noise.x * 255);
                    pNoise[1] = (uint8_t)(noise.y * 255);
                }
            }
            mpNoiseTexture = GraphicsSystem::GetInstance()->CreateTexture(
                {"Noise", NOISE_TEXTURE_WIDTH, NOISE_TEXTURE_HEIGHT, 1, 1, TextureType::TEXTURE_2D,
                 TextureUsageFlagBit::SAMPLED, TextureFormat::R8G8B8A8_UNORM, TextureFilter::POINT_FILTER,
                 TextureWrapMode::CLAMP, &pData[0]});
        }

        {
            uint8_t data[] = {255};
            mpEmptySSAOTexture = GraphicsSystem::GetInstance()->CreateTexture(
                {"Empty Ambient Occlusion Map", 1, 1, 1, 1, TextureType::TEXTURE_2D, TextureUsageFlagBit::SAMPLED,
                 TextureFormat::R8_UNORM, TextureFilter::POINT_FILTER, TextureWrapMode::CLAMP, data});
        }

        mpSSAOHighFrequencyPassShader = GraphicsSystem::GetInstance()->FindShader("SSAOHighFrequencyPass");
        assert(mpSSAOHighFrequencyPassShader != nullptr);
        mpSSAOBlurPassShader = GraphicsSystem::GetInstance()->FindShader("SSAOBlurPass");
        assert(mpSSAOBlurPassShader != nullptr);

        FASTCG_DECLARE_ENUM_BASED_CONSTEXPR_ARRAY(Tonemapper, const char *, TONEMAPPER_SHADER_NAMES, "None",
                                                  "CheapReinhard", "Reinhard", "ACES");

        for (TonemapperInt i = (TonemapperInt)Tonemapper::CHEAP_REINHARD, j = 0; i < (TonemapperInt)Tonemapper::LAST;
             ++i, ++j)
        {
            mTonemapperShaders[j] =
                GraphicsSystem::GetInstance()->FindShader(std::string(TONEMAPPER_SHADER_NAMES[i]) + "Pass");
            assert(mTonemapperShaders[j] != nullptr);
        }

        mpQuadMesh = StandardGeometries::CreateXYPlane("Fullscreen Quad", 2, 2, 1, 1, glm::vec3(0, 0, 0));
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Render(
        const Camera *pCamera, GraphicsContext *pGraphicsContext)
    {
        assert(pGraphicsContext != nullptr);

        OnRender(pCamera, pGraphicsContext);

        mLastInstanceConstantsBufferIdx = 0;
        mLastLightingConstantsBufferIdx = 0;
        mLastPCSSConstantsBufferIdx = 0;
        mLastFogConstantsBufferIdx = 0;
        mLastSceneConstantsBufferIdx = 0;
        mLastShadowMapPassConstantsBufferIdx = 0;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT,
                                    SceneConstantsT>::GetShadowMapPassConstantsBuffer()
    {
        while (mShadowMapPassConstantsBuffers.size() <= mLastShadowMapPassConstantsBufferIdx)
        {
            mShadowMapPassConstantsBuffers.emplace_back(GraphicsSystem::GetInstance()->CreateBuffer(
                {"Shadow Map Pass Constants (" + std::to_string(mShadowMapPassConstantsBuffers.size()) + ")",
                 BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC, sizeof(ShadowMapPassConstants)}));
        }
        return mShadowMapPassConstantsBuffers[mLastShadowMapPassConstantsBufferIdx++];
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT,
                                    SceneConstantsT>::GetInstanceConstantsBuffer()
    {
        while (mInstanceConstantsBuffers.size() <= mLastInstanceConstantsBufferIdx)
        {
            mInstanceConstantsBuffers.emplace_back(GraphicsSystem::GetInstance()->CreateBuffer(
                {"Instance Constants (" + std::to_string(mInstanceConstantsBuffers.size()) + ")",
                 BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC, sizeof(InstanceConstants)}));
        }
        return mInstanceConstantsBuffers[mLastInstanceConstantsBufferIdx++];
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT,
                                    SceneConstantsT>::GetLightingConstantsBuffer()
    {
        while (mLightingConstantsBuffers.size() <= mLastLightingConstantsBufferIdx)
        {
            mLightingConstantsBuffers.emplace_back(GraphicsSystem::GetInstance()->CreateBuffer(
                {"Lighting Constants (" + std::to_string(mLightingConstantsBuffers.size()) + ")",
                 BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC, sizeof(LightingConstants)}));
        }
        return mLightingConstantsBuffers[mLastLightingConstantsBufferIdx++];
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetPCSSConstantsBuffer()
    {
        while (mPCSSConstantsBuffers.size() <= mLastPCSSConstantsBufferIdx)
        {
            mPCSSConstantsBuffers.emplace_back(GraphicsSystem::GetInstance()->CreateBuffer(
                {"PCSS Constants (" + std::to_string(mPCSSConstantsBuffers.size()) + ")",
                 BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC, sizeof(PCSSConstants)}));
        }
        return mPCSSConstantsBuffers[mLastPCSSConstantsBufferIdx++];
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetFogConstantsBuffer()
    {
        while (mFogConstantsBuffers.size() <= mLastFogConstantsBufferIdx)
        {
            mFogConstantsBuffers.emplace_back(GraphicsSystem::GetInstance()->CreateBuffer(
                {"Fog Constants (" + std::to_string(mFogConstantsBuffers.size()) + ")",
                 BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC, sizeof(FogConstants)}));
        }
        return mFogConstantsBuffers[mLastFogConstantsBufferIdx++];
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetSceneConstantsBuffer()
    {
        while (mSceneConstantsBuffers.size() <= mLastSceneConstantsBufferIdx)
        {
            mSceneConstantsBuffers.emplace_back(GraphicsSystem::GetInstance()->CreateBuffer(
                {"Scene Constants (" + std::to_string(mSceneConstantsBuffers.size()) + ")",
                 BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC, sizeof(SceneConstants)}));
        }
        return mSceneConstantsBuffers[mLastSceneConstantsBufferIdx++];
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::SetGraphicsContextState(
        const GraphicsContextState &rGraphicsContextState, GraphicsContext *pGraphicsContext) const
    {
        pGraphicsContext->SetDepthTest(rGraphicsContextState.depthTest);
        pGraphicsContext->SetDepthWrite(rGraphicsContextState.depthWrite);
        pGraphicsContext->SetDepthFunc(rGraphicsContextState.depthFunc);
        pGraphicsContext->SetScissorTest(rGraphicsContextState.scissorTest);
        if (rGraphicsContextState.stencilTest)
        {
            pGraphicsContext->SetStencilTest(true);
            pGraphicsContext->SetStencilFunc(Face::BACK, rGraphicsContextState.stencilBackState.compareOp,
                                             rGraphicsContextState.stencilBackState.reference,
                                             rGraphicsContextState.stencilBackState.compareMask);
            pGraphicsContext->SetStencilOp(Face::BACK, rGraphicsContextState.stencilBackState.stencilFailOp,
                                           rGraphicsContextState.stencilBackState.depthFailOp,
                                           rGraphicsContextState.stencilBackState.passOp);
            pGraphicsContext->SetStencilWriteMask(Face::BACK, rGraphicsContextState.stencilBackState.writeMask);
            pGraphicsContext->SetStencilFunc(Face::FRONT, rGraphicsContextState.stencilFrontState.compareOp,
                                             rGraphicsContextState.stencilFrontState.reference,
                                             rGraphicsContextState.stencilFrontState.compareMask);
            pGraphicsContext->SetStencilOp(Face::FRONT, rGraphicsContextState.stencilFrontState.stencilFailOp,
                                           rGraphicsContextState.stencilFrontState.depthFailOp,
                                           rGraphicsContextState.stencilFrontState.passOp);
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
            pGraphicsContext->SetBlendFunc(rGraphicsContextState.blendState.colorOp,
                                           rGraphicsContextState.blendState.alphaOp);
            pGraphicsContext->SetBlendFactors(
                rGraphicsContextState.blendState.srcColorFactor, rGraphicsContextState.blendState.dstColorFactor,
                rGraphicsContextState.blendState.srcAlphaFactor, rGraphicsContextState.blendState.dstAlphaFactor);
        }
        else
        {
            pGraphicsContext->SetBlend(false);
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::CreateSSAORenderTargets()
    {
        mSSAORenderTargets[0] = GraphicsSystem::GetInstance()->CreateTexture(
            {"Ambient Occlusion Map", mArgs.rScreenWidth, mArgs.rScreenHeight, 1, 1, TextureType::TEXTURE_2D,
             TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET, TextureFormat::R8G8B8A8_UNORM,
             TextureFilter::LINEAR_FILTER, TextureWrapMode::CLAMP});
        mSSAORenderTargets[1] = GraphicsSystem::GetInstance()->CreateTexture(
            {"Blurred Ambient Occlusion Map", mArgs.rScreenWidth, mArgs.rScreenHeight, 1, 1, TextureType::TEXTURE_2D,
             TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET, TextureFormat::R8G8B8A8_UNORM,
             TextureFilter::LINEAR_FILTER, TextureWrapMode::CLAMP});
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
        for (auto it = mSceneConstantsBuffers.begin(); it != mSceneConstantsBuffers.end(); ++it)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(*it);
        }
        mSceneConstantsBuffers.clear();

        for (auto it = mLightingConstantsBuffers.begin(); it != mLightingConstantsBuffers.end(); ++it)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(*it);
        }
        mLightingConstantsBuffers.clear();

        for (auto it = mInstanceConstantsBuffers.begin(); it != mInstanceConstantsBuffers.end(); ++it)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(*it);
        }
        mInstanceConstantsBuffers.clear();

        for (auto it = mShadowMapPassConstantsBuffers.begin(); it != mShadowMapPassConstantsBuffers.end(); ++it)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(*it);
        }
        mShadowMapPassConstantsBuffers.clear();

        mpShadowMapPassShader = nullptr;

        if (mpEmptyShadowMap != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyTexture(mpEmptyShadowMap);
            mpEmptyShadowMap = nullptr;
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

        mTonemapperShaders = {};
    }

#if _DEBUG
    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::DebugMenuItemCallback(int &result)
    {
        if (ImGui::BeginMenu("PCSS"))
        {
            ImGui::SliderFloat("UV Scale", &mPCSSConstants.pcssData.uvScale, 0, 0.2f);
            ImGui::SliderFloat("Shadow Map Bias", &mPCSSConstants.pcssData.shadowMapData.bias, 0, 0.1f);
            ImGui::SliderInt("Blocker Search Samples", &mPCSSConstants.pcssData.blockerSearchSamples, 1, 64);
            ImGui::SliderInt("PCF Samples", &mPCSSConstants.pcssData.pcfSamples, 1, 64);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("SSAO"))
        {
            ImGui::Checkbox("Blur enabled", &mSSAOBlurEnabled);
            ImGui::DragFloat("Radius", &mSSAOHighFrequencyPassConstants.radius, 0.005f, 0.01f, 10.0f);
            ImGui::DragFloat("Bias", &mSSAOHighFrequencyPassConstants.bias, 0.0001f, 0.0001f, 1.0f);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tonemap"))
        {
            FASTCG_DECLARE_ENUM_BASED_CONSTEXPR_ARRAY(Tonemapper, const char *, TONEMAPPER_DISPLAY_NAMES, "None",
                                                      "Cheap Reinhard", "Reinhard", "ACES");

            int selectedTonemapper = (int)mTonemapper;
            if (ImGui::Combo("Tonemapper", &selectedTonemapper, TONEMAPPER_DISPLAY_NAMES,
                             FASTCG_ARRAYSIZE(TONEMAPPER_DISPLAY_NAMES)))
            {
                mTonemapper = (Tonemapper)selectedTonemapper;
            }
            ImGui::EndMenu();
        }
    }
#endif

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const typename BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::ShadowMap &
    BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetOrCreateShadowMap(
        const Light *pLight)
    {
        auto shadowMapKey = GetShadowMapKey(pLight);
        auto it = mShadowMaps.find(shadowMapKey);
        if (it == mShadowMaps.end())
        {
            it = mShadowMaps
                     .emplace(shadowMapKey,
                              ShadowMap{pLight, CreateShadowMapTexture("Shadow Map (" +
                                                                           std::to_string(mShadowMaps.size()) + ")",
                                                                       SHADOW_MAP_SIZE, SHADOW_MAP_SIZE)})
                     .first;
        }
        return it->second;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    bool BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GetShadowMap(
        const Light *pLight, ShadowMap &rShadowMap) const
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
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GenerateShadowMaps(
        GraphicsContext *pGraphicsContext)
    {
        const auto &rShadowCastersRenderBatch = mArgs.rRenderBatchStrategy.GetShadowCastersRenderBatch();
        if (rShadowCastersRenderBatch.renderablesPerMesh.empty())
        {
            return;
        }

        pGraphicsContext->PushDebugMarker("Shadow Casters Passes");
        {
            pGraphicsContext->SetViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
            pGraphicsContext->SetDepthTest(true);
            pGraphicsContext->SetDepthFunc(CompareOp::LESS);
            pGraphicsContext->SetDepthWrite(true);
            pGraphicsContext->SetScissorTest(false);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetCullMode(Face::BACK);
            pGraphicsContext->SetBlend(false);

            pGraphicsContext->BindShader(mpShadowMapPassShader);

            const auto &rDirectionalLights = WorldSystem::GetInstance()->GetDirectionalLights();
            for (size_t i = 0; i < rDirectionalLights.size(); ++i)
            {
                const auto *pDirectionalLight = rDirectionalLights[i];

                const auto &rShadowMap = GetOrCreateShadowMap(pDirectionalLight);

                const auto *pShadowMapTexture = rShadowMap.GetTexture();

                for (auto it = rShadowCastersRenderBatch.renderablesPerMesh.cbegin();
                     it != rShadowCastersRenderBatch.renderablesPerMesh.cend(); ++it)
                {
                    const auto &pMesh = it->first;
                    const auto &rRenderables = it->second;

                    pGraphicsContext->SetVertexBuffers(pMesh->GetVertexBuffers(), pMesh->GetVertexBufferCount());
                    pGraphicsContext->SetIndexBuffer(pMesh->GetIndexBuffer());

                    pGraphicsContext->SetRenderTargets(nullptr, 0, pShadowMapTexture);
                    pGraphicsContext->ClearDepthBuffer(1);

                    uint32_t instanceCount;
                    const Buffer *pShadowMapPassConstantsBuffer;
                    {
                        auto result = UpdateShadowMapPassConstants(rRenderables, rShadowMap.GetView(),
                                                                   rShadowMap.GetProjection(), pGraphicsContext);
                        instanceCount = result.first;
                        pShadowMapPassConstantsBuffer = result.second;
                    }
                    pGraphicsContext->BindResource(pShadowMapPassConstantsBuffer,
                                                   SHADOW_MAP_PASS_CONSTANTS_SHADER_RESOURCE_NAME);

                    if (instanceCount == 1)
                    {
                        pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, pMesh->GetIndexCount(), 0);
                    }
                    else
                    {
                        pGraphicsContext->DrawInstancedIndexed(PrimitiveType::TRIANGLES, 0, instanceCount, 0,
                                                               pMesh->GetIndexCount(), 0);
                    }

                    mArgs.rRenderingStatistics.drawCalls++;
                }
            }
        }
        pGraphicsContext->PopDebugMarker();
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::GenerateAmbientOcculusionMap(
        const glm::mat4 &rProjection, float fov, const Texture *pDepth, GraphicsContext *pGraphicsContext)
    {
        pGraphicsContext->PushDebugMarker("Ambient Occlusion Map Generation");
        {
            pGraphicsContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
            pGraphicsContext->SetDepthTest(false);
            pGraphicsContext->SetDepthWrite(false);
            pGraphicsContext->SetScissorTest(false);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetCullMode(Face::NONE);
            pGraphicsContext->SetBlend(false);

            pGraphicsContext->PushDebugMarker("SSAO High Frequency Pass");
            {
                pGraphicsContext->SetRenderTargets(&mSSAORenderTargets[0], 1, nullptr);
                pGraphicsContext->ClearRenderTarget(0, Colors::NONE);

                pGraphicsContext->BindShader(mpSSAOHighFrequencyPassShader);

                UpdateSSAOHighFrequencyPassConstants(rProjection, fov, pDepth, pGraphicsContext);
                pGraphicsContext->BindResource(mpSSAOHighFrequencyPassConstantsBuffer,
                                               SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_SHADER_RESOURCE_NAME);

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
                    pGraphicsContext->SetRenderTargets(&mSSAORenderTargets[1], 1, nullptr);
                    pGraphicsContext->ClearRenderTarget(0, Colors::NONE);

                    pGraphicsContext->BindShader(mpSSAOBlurPassShader);

                    pGraphicsContext->BindResource(mSSAORenderTargets[0], "uAmbientOcclusionMap");

                    pGraphicsContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(),
                                                       mpQuadMesh->GetVertexBufferCount());
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
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::RenderSkybox(
        const Texture *pRenderTarget, const Texture *pDepthScencilBuffer, const Buffer *pSceneConstantsBuffer,
        const glm::mat4 &rView, const glm::mat4 &rProjection, GraphicsContext *pGraphicsContext)
    {
        const auto &rSkyboxRenderBatch = mArgs.rRenderBatchStrategy.GetSkyboxRenderBatch();
        if (rSkyboxRenderBatch.renderablesPerMesh.empty())
        {
            return;
        }

        pGraphicsContext->PushDebugMarker("Skybox Pass");
        {
            pGraphicsContext->SetRenderTargets(&pRenderTarget, 1, pDepthScencilBuffer);

            assert(rSkyboxRenderBatch.pMaterial != nullptr);

            BindMaterial(rSkyboxRenderBatch.pMaterial, pGraphicsContext);

            // TODO: maybe validate or force skybox graphics context state?
            SetGraphicsContextState(rSkyboxRenderBatch.pMaterial->GetGraphicsContextState(), pGraphicsContext);

            pGraphicsContext->BindResource(pSceneConstantsBuffer, SCENE_CONSTANTS_SHADER_RESOURCE_NAME);

            const auto it = rSkyboxRenderBatch.renderablesPerMesh.cbegin();
            const auto &rpMesh = it->first;
            auto result = UpdateInstanceConstants(it->second, rView, rProjection, pGraphicsContext);
            assert(result.first == 1);
            const auto *pInstanceConstantsBuffer = result.second;

            pGraphicsContext->BindResource(pInstanceConstantsBuffer, INSTANCE_CONSTANTS_SHADER_RESOURCE_NAME);

            pGraphicsContext->SetVertexBuffers(rpMesh->GetVertexBuffers(), rpMesh->GetVertexBufferCount());
            pGraphicsContext->SetIndexBuffer(rpMesh->GetIndexBuffer());

            pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, rpMesh->GetIndexCount(), 0);

            mArgs.rRenderingStatistics.drawCalls++;
            mArgs.rRenderingStatistics.triangles += rpMesh->GetTriangleCount();
        }
        pGraphicsContext->PopDebugMarker();
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::Tonemap(
        const Texture *pSourceRenderTarget, const Texture *pDestinationRenderTarget, GraphicsContext *pGraphicsContext)
    {
        auto tonemapDebugMarker = std::string("Tonemapping with ") + Tonemapper_STRINGS[(TonemapperInt)mTonemapper];
        pGraphicsContext->PushDebugMarker(tonemapDebugMarker.c_str());
        {
            if (mTonemapper == Tonemapper::NONE)
            {
                pGraphicsContext->Blit(pSourceRenderTarget, pDestinationRenderTarget);
            }
            else
            {
                pGraphicsContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
                pGraphicsContext->SetDepthTest(false);
                pGraphicsContext->SetDepthWrite(false);
                pGraphicsContext->SetScissorTest(false);
                pGraphicsContext->SetStencilTest(false);
                pGraphicsContext->SetCullMode(Face::NONE);
                pGraphicsContext->SetBlend(false);
                pGraphicsContext->SetRenderTargets(&pDestinationRenderTarget, 1, nullptr);

                pGraphicsContext->BindShader(mTonemapperShaders[(TonemapperInt)mTonemapper - 1]);

                pGraphicsContext->BindResource(pSourceRenderTarget, "uSource");

                pGraphicsContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(), mpQuadMesh->GetVertexBufferCount());
                pGraphicsContext->SetIndexBuffer(mpQuadMesh->GetIndexBuffer());

                pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpQuadMesh->GetIndexCount(), 0);

                mArgs.rRenderingStatistics.drawCalls++;
            }
        }
        pGraphicsContext->PopDebugMarker();
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::BindMaterial(
        const std::shared_ptr<Material> &rpMaterial, GraphicsContext *pGraphicsContext)
    {
        const auto *pConstantBuffer = rpMaterial->GetConstantBuffer();
        if (pConstantBuffer != nullptr)
        {
            pGraphicsContext->Copy(pConstantBuffer, rpMaterial->GetConstantBufferData(),
                                   rpMaterial->GetConstantBufferSize());
        }

        pGraphicsContext->BindShader(rpMaterial->GetShader());

        if (pConstantBuffer != nullptr)
        {
            pGraphicsContext->BindResource(pConstantBuffer, MATERIAL_CONSTANTS_SHADER_RESOURCE_NAME);
        }

        for (size_t i = 0; i < rpMaterial->GetTextureCount(); ++i)
        {
            std::string name;
            const Texture *pTexture;
            rpMaterial->GetTextureAt(i, name, pTexture);
            pGraphicsContext->BindResource(pTexture, name.c_str());
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateInstanceConstants(
        const glm::mat4 &rModel, const glm::mat4 &rView, const glm::mat4 &rProjection,
        GraphicsContext *pGraphicsContext)
    {
        auto &rInstanceData = mInstanceConstants.instanceData[0];

        rInstanceData.model = rModel;
        rInstanceData.modelInverseTranspose = glm::transpose(glm::inverse(rModel));
        rInstanceData.viewProjection = rProjection * rView;
        rInstanceData.modelViewProjection = rInstanceData.viewProjection * rModel;

        const auto *pInstanceConstantsBuffer = GetInstanceConstantsBuffer();
        pGraphicsContext->Copy(pInstanceConstantsBuffer, &mInstanceConstants,
                               sizeof(InstanceConstants::instanceData[0]));
        return pInstanceConstantsBuffer;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    std::pair<uint32_t, const Buffer *> BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::
        UpdateInstanceConstants(const std::vector<const Renderable *> &rRenderables, const glm::mat4 &rView,
                                const glm::mat4 &rProjection, GraphicsContext *pGraphicsContext)
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

            rInstanceData.model = model;
            rInstanceData.modelInverseTranspose = glm::transpose(glm::inverse(model));
            rInstanceData.viewProjection = rProjection * rView;
            rInstanceData.modelViewProjection = rInstanceData.viewProjection * model;
        }

        assert(instanceCount < MAX_NUM_INSTANCES);

        const auto *pInstanceConstantsBuffer = GetInstanceConstantsBuffer();
        if (instanceCount > 0)
        {
            pGraphicsContext->Copy(pInstanceConstantsBuffer, &mInstanceConstants,
                                   sizeof(InstanceConstants::instanceData[0]) * instanceCount);
        }

        return {instanceCount, pInstanceConstantsBuffer};
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::EmptyLightingConstants(
        GraphicsContext *pGraphicsContext)
    {
        mLightingConstants = {};

        const auto *pLightingConstantsBuffer = GetLightingConstantsBuffer();
        pGraphicsContext->Copy(pLightingConstantsBuffer, &mLightingConstants, sizeof(LightingConstants));
        return pLightingConstantsBuffer;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstants(
        const PointLight *pPointLight, GraphicsContext *pGraphicsContext)
    {
        mLightingConstants.light0Position =
            glm::vec4(pPointLight->GetGameObject()->GetTransform()->GetWorldPosition(), 1);
        mLightingConstants.light0DiffuseColor = pPointLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pPointLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pPointLight->GetIntensity();
        mLightingConstants.light0ConstantAttenuation = pPointLight->GetConstantAttenuation();
        mLightingConstants.light0LinearAttenuation = pPointLight->GetLinearAttenuation();
        mLightingConstants.light0QuadraticAttenuation = pPointLight->GetQuadraticAttenuation();
        mLightingConstants.ambientColor = mArgs.rAmbientLightColor;

        const auto *pLightingConstantsBuffer = GetLightingConstantsBuffer();
        pGraphicsContext->Copy(pLightingConstantsBuffer, &mLightingConstants, sizeof(LightingConstants));
        return pLightingConstantsBuffer;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateLightingConstants(
        const DirectionalLight *pDirectionalLight, const glm::vec3 &rLightDirection, GraphicsContext *pGraphicsContext)
    {
        mLightingConstants.light0Position = glm::vec4(rLightDirection, -1);
        mLightingConstants.light0DiffuseColor = pDirectionalLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pDirectionalLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pDirectionalLight->GetIntensity();
        mLightingConstants.ambientColor = mArgs.rAmbientLightColor;

        const auto *pLightingConstantsBuffer = GetLightingConstantsBuffer();
        pGraphicsContext->Copy(pLightingConstantsBuffer, &mLightingConstants, sizeof(LightingConstants));
        return pLightingConstantsBuffer;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::EmptyPCSSConstants(
        GraphicsContext *pGraphicsContext)
    {
        mPCSSConstants = {};

        pGraphicsContext->BindResource(mpEmptyShadowMap, "uShadowMap");

        const auto *pPCSSConstantsBuffer = GetPCSSConstantsBuffer();
        pGraphicsContext->Copy(pPCSSConstantsBuffer, &mPCSSConstants, sizeof(PCSSConstants));
        return pPCSSConstantsBuffer;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdatePCSSConstants(
        const PointLight *pPointLight, float nearClip, GraphicsContext *pGraphicsContext)
    {
        FASTCG_UNUSED(pPointLight);
        FASTCG_UNUSED(nearClip);

        // TODO: implement PCSS for point lights

        mPCSSConstants.pcssData.shadowMapData.viewProjection = INVALID_SHADOW_MAP_PROJECTION;
        mPCSSConstants.pcssData.nearClip = 0;

        pGraphicsContext->BindResource(mpEmptyShadowMap, "uShadowMap");

        const auto *pPCSSConstantsBuffer = GetPCSSConstantsBuffer();
        pGraphicsContext->Copy(pPCSSConstantsBuffer, &mPCSSConstants, sizeof(PCSSConstants));
        return pPCSSConstantsBuffer;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdatePCSSConstants(
        const DirectionalLight *pDirectionalLight, float nearClip, GraphicsContext *pGraphicsContext)
    {
        ShadowMap shadowMap;
        if (GetShadowMap(pDirectionalLight, shadowMap))
        {
            mPCSSConstants.pcssData.shadowMapData.viewProjection = shadowMap.GetProjection() * shadowMap.GetView();
            mPCSSConstants.pcssData.nearClip = nearClip;

            pGraphicsContext->BindResource(shadowMap.GetTexture(), "uShadowMap");
        }
        else
        {
            mPCSSConstants.pcssData.shadowMapData.viewProjection = INVALID_SHADOW_MAP_PROJECTION;
            mPCSSConstants.pcssData.nearClip = 0;

            pGraphicsContext->BindResource(mpEmptyShadowMap, "uShadowMap");
        }

        const auto *pPCSSConstantsBuffer = GetPCSSConstantsBuffer();
        pGraphicsContext->Copy(pPCSSConstantsBuffer, &mPCSSConstants, sizeof(PCSSConstants));
        return pPCSSConstantsBuffer;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateSSAOConstants(
        bool isSSAOEnabled, GraphicsContext *pGraphicsContext) const
    {
        if (isSSAOEnabled)
        {
            if (mSSAOBlurEnabled)
            {
                pGraphicsContext->BindResource(mSSAORenderTargets[1], "uAmbientOcclusionMap");
            }
            else
            {
                pGraphicsContext->BindResource(mSSAORenderTargets[0], "uAmbientOcclusionMap");
            }
        }
        else
        {
            pGraphicsContext->BindResource(mpEmptySSAOTexture, "uAmbientOcclusionMap");
        }
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateFogConstants(
        const Fog *pFog, GraphicsContext *pGraphicsContext)
    {
        if (pFog == nullptr)
        {
            mFogConstants.fogData = FogData{{}, (uint32_t)FogMode::NONE, 0, 0, 0};
        }
        else
        {
            mFogConstants.fogData = FogData{pFog->GetColor(), (uint32_t)pFog->GetMode(), pFog->GetDensity(),
                                            pFog->GetStart(), pFog->GetEnd()};
        }

        const auto *pFogConstantsBuffer = GetFogConstantsBuffer();
        pGraphicsContext->Copy(pFogConstantsBuffer, &mFogConstants, sizeof(FogConstants));
        return pFogConstantsBuffer;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    const Buffer *BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::UpdateSceneConstants(
        const glm::mat4 &rView, const glm::mat4 &rInverseView, const glm::mat4 &rProjection,
        GraphicsContext *pGraphicsContext)
    {
        mSceneConstants.view = rView;
        mSceneConstants.inverseView = rInverseView;
        mSceneConstants.projection = rProjection;
        mSceneConstants.inverseProjection = glm::inverse(rProjection);
        mSceneConstants.screenSize = glm::vec2{mArgs.rScreenWidth, mArgs.rScreenHeight};
        mSceneConstants.pointSize = 1.0f; // TODO: provide a mechanism for users to control point size

        const auto *pSceneConstantsBuffer = GetSceneConstantsBuffer();
        pGraphicsContext->Copy(pSceneConstantsBuffer, &mSceneConstants, sizeof(SceneConstants));
        return pSceneConstantsBuffer;
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    std::pair<uint32_t, const Buffer *> BaseWorldRenderer<InstanceConstantsT, LightingConstantsT, SceneConstantsT>::
        UpdateShadowMapPassConstants(const std::vector<const Renderable *> &rRenderables, const glm::mat4 &rView,
                                     const glm::mat4 &rProjection, GraphicsContext *pGraphicsContext)
    {
        uint32_t instanceCount = 0;
        for (const auto *pRenderable : rRenderables)
        {
            if (!pRenderable->GetGameObject()->IsActive())
            {
                continue;
            }

            auto &rInstanceData = mShadowMapPassConstants.instanceData[instanceCount++];

            rInstanceData.modelViewProjection =
                rProjection * rView * pRenderable->GetGameObject()->GetTransform()->GetModel();
        }

        assert(instanceCount < MAX_NUM_INSTANCES);

        const auto *pShadowMapPassConstantsBuffer = GetShadowMapPassConstantsBuffer();
        if (instanceCount > 0)
        {
            pGraphicsContext->Copy(pShadowMapPassConstantsBuffer, &mShadowMapPassConstants,
                                   sizeof(ShadowMapPassConstants::instanceData[0]) * instanceCount);
        }

        return {instanceCount, pShadowMapPassConstantsBuffer};
    }

    template <typename InstanceConstantsT, typename LightingConstantsT, typename SceneConstantsT>
    void BaseWorldRenderer<InstanceConstantsT, LightingConstantsT,
                           SceneConstantsT>::UpdateSSAOHighFrequencyPassConstants(const glm::mat4 &rProjection,
                                                                                  float fov, const Texture *pDepth,
                                                                                  GraphicsContext *pGraphicsContext)
    {
        mSSAOHighFrequencyPassConstants.projection = rProjection;
        mSSAOHighFrequencyPassConstants.aspectRatio = mArgs.rScreenWidth / (float)mArgs.rScreenHeight;
        mSSAOHighFrequencyPassConstants.tanHalfFov = MathF::Tan((MathF::DEGREES_TO_RADIANS * fov) / 2.0f);

        pGraphicsContext->Copy(mpSSAOHighFrequencyPassConstantsBuffer, &mSSAOHighFrequencyPassConstants,
                               sizeof(SSAOHighFrequencyPassConstants));

        pGraphicsContext->BindResource(mpNoiseTexture, "uNoiseMap");
        pGraphicsContext->BindResource(pDepth, "uDepth");
    }

}