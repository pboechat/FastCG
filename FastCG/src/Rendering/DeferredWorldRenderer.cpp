#include <FastCG/World/WorldSystem.h>
#include <FastCG/Rendering/StandardGeometries.h>
#include <FastCG/Rendering/DeferredWorldRenderer.h>
#include <FastCG/Core/Math.h>

#include <type_traits>

namespace
{
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

        CreateGBuffers();

        mpStencilPassShader = GraphicsSystem::GetInstance()->FindShader("StencilPass");
        mpDirectionalLightPassShader = GraphicsSystem::GetInstance()->FindShader("DirectionalLightPass");
        mpPointLightPassShader = GraphicsSystem::GetInstance()->FindShader("PointLightPass");

        mpSphereMesh = StandardGeometries::CreateSphere("Deferred Rendering PointLight Sphere", 1, LIGHT_MESH_DETAIL);
    }

    void DeferredWorldRenderer::CreateGBuffers()
    {
        mGBuffers.resize(GraphicsSystem::GetInstance()->GetMaxSimultaneousFrames());
        for (auto &rGBuffer : mGBuffers)
        {
            rGBuffer[0] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Diffuse",
                                                                        mArgs.rScreenWidth,
                                                                        mArgs.rScreenHeight,
                                                                        1,
                                                                        1,
                                                                        TextureType::TEXTURE_2D,
                                                                        TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                                                                        TextureFormat::R8G8B8A8_UNORM,
                                                                        TextureFilter::LINEAR_FILTER,
                                                                        TextureWrapMode::CLAMP});
            rGBuffer[1] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Normal",
                                                                        mArgs.rScreenWidth,
                                                                        mArgs.rScreenHeight,
                                                                        1,
                                                                        1,
                                                                        TextureType::TEXTURE_2D,
                                                                        TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                                                                        TextureFormat::R8G8B8A8_UNORM,
                                                                        TextureFilter::LINEAR_FILTER,
                                                                        TextureWrapMode::CLAMP});
            rGBuffer[2] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Specular",
                                                                        mArgs.rScreenWidth,
                                                                        mArgs.rScreenHeight,
                                                                        1,
                                                                        1,
                                                                        TextureType::TEXTURE_2D,
                                                                        TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                                                                        TextureFormat::R8G8B8A8_UNORM,
                                                                        TextureFilter::LINEAR_FILTER,
                                                                        TextureWrapMode::CLAMP});
            rGBuffer[3] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Tangent",
                                                                        mArgs.rScreenWidth,
                                                                        mArgs.rScreenHeight,
                                                                        1,
                                                                        1,
                                                                        TextureType::TEXTURE_2D,
                                                                        TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                                                                        TextureFormat::R8G8B8A8_UNORM,
                                                                        TextureFilter::LINEAR_FILTER,
                                                                        TextureWrapMode::CLAMP});
            rGBuffer[4] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Extra Data",
                                                                        mArgs.rScreenWidth,
                                                                        mArgs.rScreenHeight,
                                                                        1,
                                                                        1,
                                                                        TextureType::TEXTURE_2D,
                                                                        TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                                                                        TextureFormat::R8G8B8A8_UNORM,
                                                                        TextureFilter::LINEAR_FILTER,
                                                                        TextureWrapMode::CLAMP});
            rGBuffer[5] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Final",
                                                                        mArgs.rScreenWidth,
                                                                        mArgs.rScreenHeight,
                                                                        1,
                                                                        1,
                                                                        TextureType::TEXTURE_2D,
                                                                        TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                                                                        TextureFormat::R8G8B8A8_UNORM,
                                                                        TextureFilter::LINEAR_FILTER,
                                                                        TextureWrapMode::CLAMP});
        }

        mDepthStencilBuffers.resize(GraphicsSystem::GetInstance()->GetMaxSimultaneousFrames());
        for (auto &rDepthStencilBuffer : mDepthStencilBuffers)
        {
            rDepthStencilBuffer = GraphicsSystem::GetInstance()->CreateTexture({"Depth",
                                                                                mArgs.rScreenWidth,
                                                                                mArgs.rScreenHeight,
                                                                                1,
                                                                                1,
                                                                                TextureType::TEXTURE_2D,
                                                                                TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                                                                                TextureFormat::X8_D24_UNORM_PACK32,
                                                                                TextureFilter::POINT_FILTER,
                                                                                TextureWrapMode::CLAMP});
        }
    }

    void DeferredWorldRenderer::DestroyGBuffers()
    {
        for (auto &rGBuffer : mGBuffers)
        {
            for (auto *pRenderTarget : rGBuffer)
            {
                if (pRenderTarget != nullptr)
                {
                    GraphicsSystem::GetInstance()->DestroyTexture(pRenderTarget);
                }
            }
        }
        mGBuffers.clear();
    }

    void DeferredWorldRenderer::BindGBuffer(GraphicsContext *pGraphicsContext) const
    {
        auto &rCurrentGBuffer = mGBuffers[GraphicsSystem::GetInstance()->GetCurrentFrame()];
        pGraphicsContext->BindResource(rCurrentGBuffer[0], "uDiffuseMap");
        pGraphicsContext->BindResource(rCurrentGBuffer[1], "uNormalMap");
        pGraphicsContext->BindResource(rCurrentGBuffer[2], "uSpecularMap");
        pGraphicsContext->BindResource(rCurrentGBuffer[3], "uTangentMap");
        pGraphicsContext->BindResource(rCurrentGBuffer[4], "uExtraData");
        auto &rCurrentDepthStencilBuffer = mDepthStencilBuffers[GraphicsSystem::GetInstance()->GetCurrentFrame()];
        pGraphicsContext->BindResource(rCurrentDepthStencilBuffer, "uDepth");
    }

    const Buffer *DeferredWorldRenderer::UpdateLightingConstants(const PointLight *pPointLight, const glm::mat4 &rInverseView, GraphicsContext *pGraphicsContext)
    {
        BindGBuffer(pGraphicsContext);
        return BaseWorldRenderer::UpdateLightingConstants(pPointLight, rInverseView, pGraphicsContext);
    }

    const Buffer *DeferredWorldRenderer::UpdateLightingConstants(const DirectionalLight *pDirectionalLight, const glm::vec3 &rViewDirection, GraphicsContext *pGraphicsContext)
    {
        BindGBuffer(pGraphicsContext);
        return BaseWorldRenderer::UpdateLightingConstants(pDirectionalLight, rViewDirection, pGraphicsContext);
    }

    void DeferredWorldRenderer::Resize()
    {
        BaseWorldRenderer::Resize();

        if (GraphicsSystem::GetInstance()->IsInitialized())
        {
            DestroyGBuffers();
            CreateGBuffers();
        }
    }

    void DeferredWorldRenderer::OnRender(const Camera *pCamera, GraphicsContext *pGraphicsContext)
    {
        assert(pGraphicsContext != nullptr);

        if (pCamera == nullptr)
        {
            return;
        }

        auto view = pCamera->GetView();
        auto inverseView = glm::inverse(view);
        auto projection = pCamera->GetProjection();

        auto &rCurrentGBuffer = mGBuffers[GraphicsSystem::GetInstance()->GetCurrentFrame()];
        auto &pCurrentDepthStencilBuffer = mDepthStencilBuffers[GraphicsSystem::GetInstance()->GetCurrentFrame()];

        pGraphicsContext->PushDebugMarker("Deferred World Rendering");
        {
            GenerateShadowMaps(pGraphicsContext);

            const auto nearClip = pCamera->GetNearClip();
            const auto isSSAOEnabled = pCamera->IsSSAOEnabled();

            if (isSSAOEnabled)
            {
                GenerateAmbientOcculusionMap(projection, pCamera->GetFieldOfView(), pCurrentDepthStencilBuffer, pGraphicsContext);
            }

            pGraphicsContext->SetViewport(0, 0, rCurrentGBuffer[0]->GetWidth(), rCurrentGBuffer[0]->GetHeight());
            pGraphicsContext->SetDepthTest(true);
            pGraphicsContext->SetDepthWrite(true);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetStencilWriteMask(Face::FRONT_AND_BACK, 0);
            pGraphicsContext->SetScissorTest(false);
            pGraphicsContext->SetCullMode(Face::BACK);
            pGraphicsContext->SetBlend(false);
            pGraphicsContext->SetRenderTargets(rCurrentGBuffer.data(), (uint32_t)rCurrentGBuffer.size(), pCurrentDepthStencilBuffer);

            pGraphicsContext->PushDebugMarker("Clear G-Buffer");
            {
                for (size_t i = 0; i < rCurrentGBuffer.size() - 1; ++i)
                {
                    pGraphicsContext->ClearRenderTarget((uint32_t)i, Colors::NONE);
                }
                pGraphicsContext->ClearDepthStencilBuffer(1, 0);
            }
            pGraphicsContext->PopDebugMarker();

            const auto *pSceneConstantsBuffer = UpdateSceneConstants(view, inverseView, projection, pGraphicsContext);

            auto renderBatchIt = mArgs.rRenderBatches.cbegin() + 1;
            if (renderBatchIt != mArgs.rRenderBatches.cend())
            {
                pGraphicsContext->PushDebugMarker("Geometry Passes");
                {
                    for (; renderBatchIt != mArgs.rRenderBatches.cend(); ++renderBatchIt)
                    {
                        const auto *pMaterial = renderBatchIt->pMaterial;

                        pGraphicsContext->PushDebugMarker((pMaterial->GetName() + " Pass").c_str());
                        {
                            assert(renderBatchIt->type != RenderBatchType::SHADOW_CASTERS);

                            BindMaterial(pMaterial, pGraphicsContext);

                            pGraphicsContext->BindResource(pSceneConstantsBuffer, SCENE_CONSTANTS_SHADER_RESOURCE_NAME);

                            for (auto it = renderBatchIt->renderablesPerMesh.cbegin(); it != renderBatchIt->renderablesPerMesh.cend(); ++it)
                            {
                                const auto *pMesh = it->first;
                                const auto &rRenderables = it->second;

                                uint32_t instanceCount;
                                const Buffer *pInstanceConstantsBuffer;
                                {
                                    auto result = UpdateInstanceConstants(rRenderables, view, projection, pGraphicsContext);
                                    instanceCount = result.first;
                                    pInstanceConstantsBuffer = result.second;
                                }

                                if (instanceCount == 0)
                                {
                                    continue;
                                }

                                pGraphicsContext->BindResource(pInstanceConstantsBuffer, INSTANCE_CONSTANTS_SHADER_RESOURCE_NAME);

                                pGraphicsContext->SetVertexBuffers(pMesh->GetVertexBuffers(), pMesh->GetVertexBufferCount());
                                pGraphicsContext->SetIndexBuffer(pMesh->GetIndexBuffer());

                                if (instanceCount == 0)
                                {
                                    pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, pMesh->GetIndexCount(), 0);
                                }
                                else
                                {
                                    pGraphicsContext->DrawInstancedIndexed(PrimitiveType::TRIANGLES, 0, instanceCount, 0, pMesh->GetIndexCount(), 0);
                                }

                                mArgs.rRenderingStatistics.drawCalls++;
                                mArgs.rRenderingStatistics.triangles += pMesh->GetTriangleCount();
                            }
                        }
                        pGraphicsContext->PopDebugMarker();
                    }
                }
                pGraphicsContext->PopDebugMarker();
            }

            pGraphicsContext->PushDebugMarker("Clear G-Buffer Final Render Target");
            {
                pGraphicsContext->SetRenderTargets(&rCurrentGBuffer[5], 1, nullptr);
                pGraphicsContext->ClearRenderTarget(0, Colors::NONE);
            }
            pGraphicsContext->PopDebugMarker();

            const auto *pFogConstantsBuffer = UpdateFogConstants(WorldSystem::GetInstance()->GetFog(), pGraphicsContext);

            pGraphicsContext->PushDebugMarker("Point Light Passes");
            {
                const auto &rPointLights = WorldSystem::GetInstance()->GetPointLights();
                for (size_t i = 0; i < rPointLights.size(); i++)
                {
                    const auto *pPointLight = rPointLights[i];

                    pGraphicsContext->PushDebugMarker((std::string("Point Light Pass (") + std::to_string(i) + ")").c_str());
                    {
                        auto model = glm::scale(pPointLight->GetGameObject()->GetTransform()->GetModel(), glm::vec3(CalculateLightBoundingSphereScale(pPointLight)));

                        const auto *pInstanceConstantsBuffer = UpdateInstanceConstants(model, view, projection, pGraphicsContext);

                        pGraphicsContext->PushDebugMarker((std::string("Point Light (") + std::to_string(i) + ") Stencil Sub-Pass").c_str());
                        {
                            pGraphicsContext->SetRenderTargets(nullptr, 0, pCurrentDepthStencilBuffer);
                            pGraphicsContext->ClearStencilBuffer(0);
                            pGraphicsContext->SetStencilWriteMask(Face::FRONT_AND_BACK, 0xff);
                            pGraphicsContext->SetDepthTest(true);
                            pGraphicsContext->SetDepthWrite(false);
                            pGraphicsContext->SetStencilTest(true);
                            pGraphicsContext->SetStencilFunc(Face::FRONT_AND_BACK, CompareOp::ALWAYS, 0, 0);
                            pGraphicsContext->SetStencilOp(Face::BACK, StencilOp::KEEP, StencilOp::INCREMENT_AND_CLAMP, StencilOp::KEEP);
                            pGraphicsContext->SetStencilOp(Face::FRONT, StencilOp::KEEP, StencilOp::DECREMENT_AND_CLAMP, StencilOp::KEEP);
                            pGraphicsContext->SetBlend(false);
                            pGraphicsContext->SetCullMode(Face::NONE);

                            pGraphicsContext->BindShader(mpStencilPassShader);

                            pGraphicsContext->BindResource(pInstanceConstantsBuffer, INSTANCE_CONSTANTS_SHADER_RESOURCE_NAME);

                            pGraphicsContext->SetVertexBuffers(mpSphereMesh->GetVertexBuffers(), mpSphereMesh->GetVertexBufferCount());
                            pGraphicsContext->SetIndexBuffer(mpSphereMesh->GetIndexBuffer());

                            pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpSphereMesh->GetIndexCount(), 0);

                            mArgs.rRenderingStatistics.drawCalls++;
                        }
                        pGraphicsContext->PopDebugMarker();

                        pGraphicsContext->PushDebugMarker((std::string("Point Light (") + std::to_string(i) + ") Color Sub-Pass").c_str());
                        {
                            pGraphicsContext->SetRenderTargets(&rCurrentGBuffer[5], 1, pCurrentDepthStencilBuffer);
                            pGraphicsContext->SetDepthTest(false);
                            pGraphicsContext->SetDepthWrite(false);
                            pGraphicsContext->SetStencilTest(true);
                            pGraphicsContext->SetStencilWriteMask(Face::FRONT_AND_BACK, 0);
                            pGraphicsContext->SetStencilFunc(Face::FRONT_AND_BACK, CompareOp::NOT_EQUAL, 0, 0xff);
                            pGraphicsContext->SetBlend(true);
                            pGraphicsContext->SetBlendFunc(BlendFunc::ADD, BlendFunc::NONE);
                            pGraphicsContext->SetBlendFactors(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ZERO, BlendFactor::ZERO);
                            pGraphicsContext->SetCullMode(Face::FRONT);

                            pGraphicsContext->BindShader(mpPointLightPassShader);

                            pGraphicsContext->BindResource(pSceneConstantsBuffer, SCENE_CONSTANTS_SHADER_RESOURCE_NAME);
                            pGraphicsContext->BindResource(pFogConstantsBuffer, FOG_CONSTANTS_SHADER_RESOURCE_NAME);

                            UpdateSSAOConstants(isSSAOEnabled, pGraphicsContext);

                            const auto *pInstanceConstantsBuffer = UpdateInstanceConstants(model, view, projection, pGraphicsContext);
                            pGraphicsContext->BindResource(pInstanceConstantsBuffer, INSTANCE_CONSTANTS_SHADER_RESOURCE_NAME);

                            const auto *pLightingConstantsBuffer = UpdateLightingConstants(pPointLight, inverseView, pGraphicsContext);
                            pGraphicsContext->BindResource(pLightingConstantsBuffer, LIGHTING_CONSTANTS_SHADER_RESOURCE_NAME);

                            const auto *pPCSSConstantsBuffer = UpdatePCSSConstants(pPointLight, nearClip, pGraphicsContext);
                            pGraphicsContext->BindResource(pPCSSConstantsBuffer, PCSS_CONSTANTS_SHADER_RESOURCE_NAME);

                            pGraphicsContext->SetVertexBuffers(mpSphereMesh->GetVertexBuffers(), mpSphereMesh->GetVertexBufferCount());
                            pGraphicsContext->SetIndexBuffer(mpSphereMesh->GetIndexBuffer());

                            pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpSphereMesh->GetIndexCount(), 0);

                            mArgs.rRenderingStatistics.drawCalls++;
                        }
                        pGraphicsContext->PopDebugMarker();
                    }
                    pGraphicsContext->PopDebugMarker();
                }
            }
            pGraphicsContext->PopDebugMarker();

            pGraphicsContext->PushDebugMarker("Directional Light Passes");
            {
                pGraphicsContext->SetRenderTargets(&rCurrentGBuffer[5], 1, nullptr);
                pGraphicsContext->SetDepthTest(false);
                pGraphicsContext->SetDepthWrite(false);
                pGraphicsContext->SetStencilTest(false);
                pGraphicsContext->SetStencilWriteMask(Face::FRONT_AND_BACK, 0);
                pGraphicsContext->SetBlend(true);
                pGraphicsContext->SetBlendFunc(BlendFunc::ADD, BlendFunc::NONE);
                pGraphicsContext->SetBlendFactors(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ZERO, BlendFactor::ZERO);
                pGraphicsContext->SetCullMode(Face::BACK);

                pGraphicsContext->BindShader(mpDirectionalLightPassShader);

                pGraphicsContext->BindResource(pSceneConstantsBuffer, SCENE_CONSTANTS_SHADER_RESOURCE_NAME);
                pGraphicsContext->BindResource(pFogConstantsBuffer, FOG_CONSTANTS_SHADER_RESOURCE_NAME);

                UpdateSSAOConstants(isSSAOEnabled, pGraphicsContext);

                auto viewTranspose = glm::transpose(glm::toMat3(pCamera->GetGameObject()->GetTransform()->GetWorldRotation()));
                const auto &rDirectionalLights = WorldSystem::GetInstance()->GetDirectionalLights();
                for (size_t i = 0; i < rDirectionalLights.size(); i++)
                {
                    const auto *pDirectionalLight = rDirectionalLights[i];
                    pGraphicsContext->PushDebugMarker((std::string("Directional Light Pass (") + std::to_string(i) + ")").c_str());
                    {
                        const auto *pLightingConstantsBuffer = UpdateLightingConstants(pDirectionalLight, glm::normalize(viewTranspose * pDirectionalLight->GetDirection()), pGraphicsContext);
                        pGraphicsContext->BindResource(pLightingConstantsBuffer, LIGHTING_CONSTANTS_SHADER_RESOURCE_NAME);
                        const auto *pPCSSConstantsBuffer = UpdatePCSSConstants(pDirectionalLight, nearClip, pGraphicsContext);
                        pGraphicsContext->BindResource(pPCSSConstantsBuffer, PCSS_CONSTANTS_SHADER_RESOURCE_NAME);

                        pGraphicsContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(), mpQuadMesh->GetVertexBufferCount());
                        pGraphicsContext->SetIndexBuffer(mpQuadMesh->GetIndexBuffer());

                        pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpQuadMesh->GetIndexCount(), 0);

                        mArgs.rRenderingStatistics.drawCalls++;
                    }
                    pGraphicsContext->PopDebugMarker();
                }
            }
            pGraphicsContext->PopDebugMarker();

            pGraphicsContext->PushDebugMarker("Blit G-Buffer Final Render Target into Backbuffer");
            {
                pGraphicsContext->Blit(rCurrentGBuffer[5], GraphicsSystem::GetInstance()->GetBackbuffer());
            }
            pGraphicsContext->PopDebugMarker();
        }
        pGraphicsContext->PopDebugMarker();
    }

    void DeferredWorldRenderer::Finalize()
    {
        mpSphereMesh = nullptr;

        DestroyGBuffers();

        BaseWorldRenderer::Finalize();
    }
}