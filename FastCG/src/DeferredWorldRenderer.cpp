#include <FastCG/StandardGeometries.h>
#include <FastCG/Random.h>
#include <FastCG/MathT.h>
#include <FastCG/DeferredWorldRenderer.h>

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

    float CalculateLightBoundingSphereScale(const FastCG::PointLight *pPointLight)
    {
        auto uDiffuseColor = pPointLight->GetDiffuseColor();
        auto maxChannel = FastCG::MathF::Max(FastCG::MathF::Max(uDiffuseColor.r, uDiffuseColor.g), uDiffuseColor.b);
        auto c = maxChannel * pPointLight->GetIntensity();
        return (8.0f * FastCG::MathF::Sqrt(c) + 1.0f);
    }

}

namespace FastCG
{
    constexpr uint32_t LIGHT_MESH_DETAIL = 20;

    void DeferredWorldRenderer::Initialize()
    {
        BaseWorldRenderer::Initialize();

        mGBufferRenderTargets[0] = RenderingSystem::GetInstance()->CreateTexture({"G-Buffer Diffuse",
                                                                                  mArgs.rScreenWidth,
                                                                                  mArgs.rScreenHeight,
                                                                                  TextureType::TEXTURE_2D,
                                                                                  TextureFormat::RGBA,
                                                                                  {32, 32, 32, 32},
                                                                                  TextureDataType::FLOAT,
                                                                                  TextureFilter::LINEAR_FILTER,
                                                                                  TextureWrapMode::CLAMP,
                                                                                  false});
        mGBufferRenderTargets[1] = RenderingSystem::GetInstance()->CreateTexture({"G-Buffer Normal",
                                                                                  mArgs.rScreenWidth,
                                                                                  mArgs.rScreenHeight,
                                                                                  TextureType::TEXTURE_2D,
                                                                                  TextureFormat::RGBA,
                                                                                  {32, 32, 32, 32},
                                                                                  TextureDataType::FLOAT,
                                                                                  TextureFilter::LINEAR_FILTER,
                                                                                  TextureWrapMode::CLAMP,
                                                                                  false});
        mGBufferRenderTargets[2] = RenderingSystem::GetInstance()->CreateTexture({"G-Buffer Specular",
                                                                                  mArgs.rScreenWidth,
                                                                                  mArgs.rScreenHeight,
                                                                                  TextureType::TEXTURE_2D,
                                                                                  TextureFormat::RGBA,
                                                                                  {32, 32, 32, 32},
                                                                                  TextureDataType::FLOAT,
                                                                                  TextureFilter::LINEAR_FILTER,
                                                                                  TextureWrapMode::CLAMP,
                                                                                  false});
        mGBufferRenderTargets[3] = RenderingSystem::GetInstance()->CreateTexture({"G-Buffer Tangent",
                                                                                  mArgs.rScreenWidth,
                                                                                  mArgs.rScreenHeight,
                                                                                  TextureType::TEXTURE_2D,
                                                                                  TextureFormat::RGBA,
                                                                                  {32, 32, 32, 32},
                                                                                  TextureDataType::FLOAT,
                                                                                  TextureFilter::LINEAR_FILTER,
                                                                                  TextureWrapMode::CLAMP,
                                                                                  false});
        mGBufferRenderTargets[4] = RenderingSystem::GetInstance()->CreateTexture({"G-Buffer Extra Data",
                                                                                  mArgs.rScreenWidth,
                                                                                  mArgs.rScreenHeight,
                                                                                  TextureType::TEXTURE_2D,
                                                                                  TextureFormat::RGBA,
                                                                                  {32, 32, 32, 32},
                                                                                  TextureDataType::FLOAT,
                                                                                  TextureFilter::LINEAR_FILTER,
                                                                                  TextureWrapMode::CLAMP,
                                                                                  false});
        mGBufferRenderTargets[5] = RenderingSystem::GetInstance()->CreateTexture({"G-Buffer Depth",
                                                                                  mArgs.rScreenWidth,
                                                                                  mArgs.rScreenHeight,
                                                                                  TextureType::TEXTURE_2D,
                                                                                  TextureFormat::DEPTH_STENCIL,
                                                                                  {24, 8},
                                                                                  TextureDataType::UNSIGNED_INT,
                                                                                  TextureFilter::POINT_FILTER,
                                                                                  TextureWrapMode::CLAMP,
                                                                                  false});
        mGBufferRenderTargets[6] = RenderingSystem::GetInstance()->CreateTexture({"G-Buffer Final",
                                                                                  mArgs.rScreenWidth,
                                                                                  mArgs.rScreenHeight,
                                                                                  TextureType::TEXTURE_2D,
                                                                                  TextureFormat::RGBA,
                                                                                  {32, 32, 32, 32},
                                                                                  TextureDataType::FLOAT,
                                                                                  TextureFilter::LINEAR_FILTER,
                                                                                  TextureWrapMode::CLAMP,
                                                                                  false});
        mSSAORenderTargets[0] = RenderingSystem::GetInstance()->CreateTexture({"Ambient Occlusion",
                                                                               mArgs.rScreenWidth,
                                                                               mArgs.rScreenHeight,
                                                                               TextureType::TEXTURE_2D,
                                                                               TextureFormat::RGBA,
                                                                               {32, 32, 32, 32},
                                                                               TextureDataType::FLOAT,
                                                                               TextureFilter::LINEAR_FILTER,
                                                                               TextureWrapMode::CLAMP,
                                                                               false});
        mSSAORenderTargets[1] = RenderingSystem::GetInstance()->CreateTexture({"Blurred Ambient Occlusion",
                                                                               mArgs.rScreenWidth,
                                                                               mArgs.rScreenHeight,
                                                                               TextureType::TEXTURE_2D,
                                                                               TextureFormat::RGBA,
                                                                               {32, 32, 32, 32},
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

        mpStencilPassShader = RenderingSystem::GetInstance()->FindShader("StencilPass");
        mpDirectionalLightPassShader = RenderingSystem::GetInstance()->FindShader("DirectionalLightPass");
        mpPointLightPassShader = RenderingSystem::GetInstance()->FindShader("PointLightPass");
        mpSSAOHighFrequencyPassShader = RenderingSystem::GetInstance()->FindShader("SSAOHighFrequencyPass");
        mpSSAOBlurPassShader = RenderingSystem::GetInstance()->FindShader("SSAOBlurPass");

        mpQuadMesh = StandardGeometries::CreateXYPlane("Deferred Rendering Fullscreen Quad", 2, 2, 1, 1, glm::vec3(0, 0, 0));
        mpSphereMesh = StandardGeometries::CreateSphere("Deferred Rendering PointLight Sphere", 1, LIGHT_MESH_DETAIL);
    }

    void DeferredWorldRenderer::BindGBufferTextures(RenderingContext *pRenderingContext) const
    {
        pRenderingContext->Bind(mGBufferRenderTargets[0], "uDiffuseMap", 3);
        pRenderingContext->Bind(mGBufferRenderTargets[1], "uNormalMap", 4);
        pRenderingContext->Bind(mGBufferRenderTargets[2], "uSpecularMap", 5);
        pRenderingContext->Bind(mGBufferRenderTargets[3], "uTangentMap", 6);
        pRenderingContext->Bind(mGBufferRenderTargets[4], "uExtraData", 7);
        pRenderingContext->Bind(mGBufferRenderTargets[5], "uDepth", 8);
    }

    void DeferredWorldRenderer::BindSSAOTexture(bool isSSAOEnabled, RenderingContext *pRenderingContext) const
    {
        if (isSSAOEnabled)
        {
            if (mSSAOBlurEnabled)
            {
                pRenderingContext->Bind(mSSAORenderTargets[1], "uAmbientOcclusionMap", 9);
            }
            else
            {
                pRenderingContext->Bind(mSSAORenderTargets[0], "uAmbientOcclusionMap", 9);
            }
        }
        else
        {
            pRenderingContext->Bind(mpEmptySSAOTexture, "uAmbientOcclusionMap", 9);
        }
    }

    void DeferredWorldRenderer::UpdateSceneConstants(const glm::mat4 &rView, const glm::mat4 &rProjection, float fov, RenderingContext *pRenderingContext)
    {
        mSceneConstants.screenSize = glm::vec2{mArgs.rScreenWidth, mArgs.rScreenHeight};
        mSceneConstants.aspectRatio = mArgs.rScreenWidth / (float)mArgs.rScreenHeight;
        mSceneConstants.tanHalfFov = MathF::Tan((MathF::DEGREES_TO_RADIANS * fov) / 2.0f);
        BaseWorldRenderer::UpdateSceneConstants(rView, rProjection, pRenderingContext);
    }

    void DeferredWorldRenderer::UpdateLightingConstants(const PointLight *pPointLight, const glm::mat4 &rView, bool isSSAOEnabled, RenderingContext *pRenderingContext)
    {
        BindGBufferTextures(pRenderingContext);
        BindSSAOTexture(isSSAOEnabled, pRenderingContext);
        BaseWorldRenderer::UpdateLightingConstants(pPointLight, rView, pRenderingContext);
    }

    void DeferredWorldRenderer::UpdateLightingConstants(const DirectionalLight *pDirectionalLight, const glm::vec3 &rDirection, bool isSSAOEnabled, RenderingContext *pRenderingContext)
    {
        BindGBufferTextures(pRenderingContext);
        BindSSAOTexture(isSSAOEnabled, pRenderingContext);
        BaseWorldRenderer::UpdateLightingConstants(pDirectionalLight, rDirection, pRenderingContext);
    }

    void DeferredWorldRenderer::Render(const Camera *pCamera, RenderingContext *pRenderingContext)
    {
        assert(pCamera != nullptr);
        assert(pRenderingContext != nullptr);

        auto view = pCamera->GetView();
        auto projection = pCamera->GetProjection();

        pRenderingContext->Begin();
        {
            pRenderingContext->PushDebugMarker("Deferred World Rendering");
            {
                GenerateShadowMaps(pRenderingContext);

                pRenderingContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
                pRenderingContext->SetDepthTest(true);
                pRenderingContext->SetDepthWrite(true);
                pRenderingContext->SetStencilTest(false);
                pRenderingContext->SetStencilWriteMask(0);
                pRenderingContext->SetScissorTest(false);
                pRenderingContext->SetCullMode(Face::BACK);
                pRenderingContext->SetBlend(false);
                pRenderingContext->SetRenderTargets(mGBufferRenderTargets.data(), 6);

                pRenderingContext->PushDebugMarker("Clear G-Buffer");
                {
                    for (auto i : {0, 1, 2, 3, 4})
                    {
                        pRenderingContext->ClearRenderTarget(i, Colors::NONE);
                    }
                    pRenderingContext->ClearDepthStencilTarget(0, 1, 0);
                }
                pRenderingContext->PopDebugMarker();

                auto renderBatchIt = mArgs.rRenderBatches.cbegin() + 1;
                if (renderBatchIt != mArgs.rRenderBatches.cend())
                {
                    pRenderingContext->PushDebugMarker("Material Passes");
                    {
                        UpdateSceneConstants(view, projection, pCamera->GetFieldOfView(), pRenderingContext);

                        for (; renderBatchIt != mArgs.rRenderBatches.cend(); ++renderBatchIt)
                        {
                            const auto *pMaterial = renderBatchIt->pMaterial;

                            pRenderingContext->PushDebugMarker((pMaterial->GetName() + " Pass").c_str());
                            {
                                assert(renderBatchIt->type == RenderBatchType::MATERIAL_BASED);

                                SetupMaterial(pMaterial, pRenderingContext);

                                pRenderingContext->Bind(mpSceneConstantsBuffer, Shader::SCENE_CONSTANTS_BINDING_INDEX);

                                for (const auto *pRenderable : renderBatchIt->renderables)
                                {
                                    if (!pRenderable->GetGameObject()->IsActive())
                                    {
                                        continue;
                                    }

                                    UpdateInstanceConstants(pRenderable->GetGameObject()->GetTransform()->GetModel(), mSceneConstants.view, mSceneConstants.projection, pRenderingContext);
                                    pRenderingContext->Bind(mpInstanceConstantsBuffer, Shader::INSTANCE_CONTANTS_BINDING_INDEX);

                                    auto *pMesh = pRenderable->GetMesh();

                                    pRenderingContext->SetVertexBuffers(pMesh->GetVertexBuffers(), pMesh->GetVertexBufferCount());
                                    pRenderingContext->SetIndexBuffer(pMesh->GetIndexBuffer());

                                    pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, pMesh->GetIndexCount(), 0, 0);

                                    mArgs.rRenderingStatistics.drawCalls++;
                                    mArgs.rRenderingStatistics.triangles += pMesh->GetTriangleCount();
                                }
                            }
                            pRenderingContext->PopDebugMarker();
                        }
                    }
                    pRenderingContext->PopDebugMarker();
                }

                {
                    const bool isSSAOEnabled = pCamera->IsSSAOEnabled();

                    if (isSSAOEnabled)
                    {
                        pRenderingContext->PushDebugMarker("SSAO High Frequency Pass");
                        {
                            pRenderingContext->SetRenderTargets(&mSSAORenderTargets[0], 1);
                            pRenderingContext->ClearRenderTarget(0, Colors::NONE);

                            pRenderingContext->SetDepthTest(false);
                            pRenderingContext->SetDepthWrite(false);

                            pRenderingContext->Bind(mpSSAOHighFrequencyPassShader);

                            pRenderingContext->Bind(mpSceneConstantsBuffer, Shader::SCENE_CONSTANTS_BINDING_INDEX);
                            pRenderingContext->Copy(mpSSAOHighFrequencyPassConstantsBuffer, sizeof(SSAOHighFrequencyPassConstants), &mSSAOHighFrequencyPassConstants);
                            pRenderingContext->Bind(mpSSAOHighFrequencyPassConstantsBuffer, Shader::SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_BINDING_INDEX);
                            pRenderingContext->Bind(mpNoiseTexture, "uNoise", 0);
                            pRenderingContext->Bind(mGBufferRenderTargets[1], "uNormalMap", 1);
                            pRenderingContext->Bind(mGBufferRenderTargets[5], "uDepth", 2);

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

                    {
                        pRenderingContext->PushDebugMarker("Clear G-Buffer Final Render Target");
                        {
                            pRenderingContext->SetRenderTargets(&mGBufferRenderTargets[6], 1);
                            pRenderingContext->ClearRenderTarget(0, Colors::NONE);
                        }
                        pRenderingContext->PopDebugMarker();

                        pRenderingContext->PushDebugMarker("Point Light Passes");
                        {
                            for (size_t i = 0; i < mArgs.rPointLights.size(); i++)
                            {
                                const auto *pPointLight = mArgs.rPointLights[i];

                                pRenderingContext->PushDebugMarker((std::string("Point Light Pass (") + std::to_string(i) + ")").c_str());
                                {
                                    auto model = glm::scale(pPointLight->GetGameObject()->GetTransform()->GetModel(), glm::vec3(CalculateLightBoundingSphereScale(pPointLight)));

                                    UpdateInstanceConstants(model, mSceneConstants.view, mSceneConstants.projection, pRenderingContext);

                                    pRenderingContext->PushDebugMarker((std::string("Point Light (") + std::to_string(i) + ") Stencil Sub-Pass").c_str());
                                    {
                                        pRenderingContext->SetRenderTargets(&mGBufferRenderTargets[5], 1);
                                        pRenderingContext->ClearStencilTarget(0, 0);

                                        pRenderingContext->SetStencilWriteMask(0xff);
                                        pRenderingContext->SetDepthTest(true);
                                        pRenderingContext->SetDepthWrite(false);
                                        pRenderingContext->SetStencilTest(true);
                                        pRenderingContext->SetStencilFunc(Face::FRONT_AND_BACK, CompareOp::ALWAYS, 0, 0);
                                        pRenderingContext->SetStencilOp(Face::BACK, StencilOp::KEEP, StencilOp::INCREMENT_AND_CLAMP, StencilOp::KEEP);
                                        pRenderingContext->SetStencilOp(Face::FRONT, StencilOp::KEEP, StencilOp::DECREMENT_AND_CLAMP, StencilOp::KEEP);
                                        pRenderingContext->SetBlend(false);
                                        pRenderingContext->SetCullMode(Face::NONE);

                                        pRenderingContext->Bind(mpStencilPassShader);

                                        pRenderingContext->Bind(mpInstanceConstantsBuffer, Shader::INSTANCE_CONTANTS_BINDING_INDEX);

                                        pRenderingContext->SetVertexBuffers(mpSphereMesh->GetVertexBuffers(), mpSphereMesh->GetVertexBufferCount());
                                        pRenderingContext->SetIndexBuffer(mpSphereMesh->GetIndexBuffer());

                                        pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, mpSphereMesh->GetIndexCount(), 0, 0);

                                        mArgs.rRenderingStatistics.drawCalls++;
                                    }
                                    pRenderingContext->PopDebugMarker();

                                    pRenderingContext->PushDebugMarker((std::string("Point Light (") + std::to_string(i) + ") Color Sub-Pass").c_str());
                                    {
                                        pRenderingContext->SetRenderTargets(&mGBufferRenderTargets[5], 2);

                                        pRenderingContext->SetDepthTest(false);
                                        pRenderingContext->SetDepthWrite(false);
                                        pRenderingContext->SetStencilTest(true);
                                        pRenderingContext->SetStencilWriteMask(0);
                                        pRenderingContext->SetStencilFunc(Face::FRONT_AND_BACK, CompareOp::NOT_EQUAL, 0, 0xff);
                                        pRenderingContext->SetBlend(true);
                                        pRenderingContext->SetBlendFunc(BlendFunc::ADD, BlendFunc::NONE);
                                        pRenderingContext->SetBlendFactors(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ZERO, BlendFactor::ZERO);
                                        pRenderingContext->SetCullMode(Face::FRONT);

                                        pRenderingContext->Bind(mpPointLightPassShader);
                                        pRenderingContext->Bind(mpSceneConstantsBuffer, Shader::SCENE_CONSTANTS_BINDING_INDEX);
                                        UpdateInstanceConstants(model, mSceneConstants.view, mSceneConstants.projection, pRenderingContext);
                                        pRenderingContext->Bind(mpInstanceConstantsBuffer, Shader::INSTANCE_CONTANTS_BINDING_INDEX);
                                        UpdateLightingConstants(pPointLight, view, isSSAOEnabled, pRenderingContext);
                                        pRenderingContext->Bind(mpLightingConstantsBuffer, Shader::LIGHTING_CONSTANTS_BINDING_INDEX);

                                        pRenderingContext->SetVertexBuffers(mpSphereMesh->GetVertexBuffers(), mpSphereMesh->GetVertexBufferCount());
                                        pRenderingContext->SetIndexBuffer(mpSphereMesh->GetIndexBuffer());

                                        pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, mpSphereMesh->GetIndexCount(), 0, 0);

                                        mArgs.rRenderingStatistics.drawCalls++;
                                    }
                                    pRenderingContext->PopDebugMarker();
                                }
                                pRenderingContext->PopDebugMarker();
                            }
                        }
                        pRenderingContext->PopDebugMarker();

                        pRenderingContext->PushDebugMarker("Directional Light Passes");
                        {
                            pRenderingContext->SetRenderTargets(&mGBufferRenderTargets[6], 1);

                            pRenderingContext->SetDepthTest(false);
                            pRenderingContext->SetDepthWrite(false);
                            pRenderingContext->SetStencilTest(false);
                            pRenderingContext->SetStencilWriteMask(0);
                            pRenderingContext->SetBlend(true);
                            pRenderingContext->SetBlendFunc(BlendFunc::ADD, BlendFunc::NONE);
                            pRenderingContext->SetBlendFactors(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ZERO, BlendFactor::ZERO);
                            pRenderingContext->SetCullMode(Face::BACK);

                            pRenderingContext->Bind(mpDirectionalLightPassShader);

                            auto inverseCameraRotation = glm::inverse(pCamera->GetGameObject()->GetTransform()->GetRotation());
                            for (size_t i = 0; i < mArgs.rDirectionalLights.size(); i++)
                            {
                                const auto *pDirectionalLight = mArgs.rDirectionalLights[i];
                                pRenderingContext->PushDebugMarker((std::string("Directional Light Pass (") + std::to_string(i) + ")").c_str());
                                {
                                    UpdateLightingConstants(pDirectionalLight, glm::vec3(glm::normalize(inverseCameraRotation * glm::vec4(pDirectionalLight->GetDirection(), 1))), isSSAOEnabled, pRenderingContext);
                                    pRenderingContext->Bind(mpLightingConstantsBuffer, Shader::LIGHTING_CONSTANTS_BINDING_INDEX);

                                    pRenderingContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(), mpQuadMesh->GetVertexBufferCount());
                                    pRenderingContext->SetIndexBuffer(mpQuadMesh->GetIndexBuffer());

                                    pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, mpQuadMesh->GetIndexCount(), 0, 0);

                                    mArgs.rRenderingStatistics.drawCalls++;
                                }
                                pRenderingContext->PopDebugMarker();
                            }
                        }
                        pRenderingContext->PopDebugMarker();

                        pRenderingContext->PushDebugMarker("Blit G-Buffer Final Render Target into Color Backbuffer");
                        {
                            pRenderingContext->Blit(mGBufferRenderTargets[6], RenderingSystem::GetInstance()->GetBackbuffer());
                        }
                        pRenderingContext->PopDebugMarker();
                    }
                }
            }
            pRenderingContext->PopDebugMarker();
        }
        pRenderingContext->End();
    }

    void DeferredWorldRenderer::Finalize()
    {
        if (mpSphereMesh != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyMesh(mpSphereMesh);
        }

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

        for (const auto *pRenderTarget : mSSAORenderTargets)
        {
            RenderingSystem::GetInstance()->DestroyTexture(pRenderTarget);
        }
        memset(mSSAORenderTargets.data(), 0, sizeof(mSSAORenderTargets));

        for (const auto *pRenderTarget : mGBufferRenderTargets)
        {
            RenderingSystem::GetInstance()->DestroyTexture(pRenderTarget);
        }
        memset(mGBufferRenderTargets.data(), 0, sizeof(mGBufferRenderTargets));

        if (mpSSAOHighFrequencyPassConstantsBuffer != nullptr)
        {
            RenderingSystem::GetInstance()->DestroyBuffer(mpSSAOHighFrequencyPassConstantsBuffer);
            mpSSAOHighFrequencyPassConstantsBuffer = nullptr;
        }

        BaseWorldRenderer::Finalize();
    }
}