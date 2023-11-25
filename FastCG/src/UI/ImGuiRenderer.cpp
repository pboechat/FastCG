#include <FastCG/UI/ImGuiRenderer.h>

#include <glm/gtc/matrix_transform.hpp>

#include <cstdint>

namespace FastCG
{
    static_assert(sizeof(ImDrawIdx) == sizeof(uint32_t), "Please configure ImGui to use 32-bit indices");

    // TODO: make this less brittle and possibly dynamic
    constexpr size_t MAX_NUM_VERTICES = 100000;

    void ImGuiRenderer::Initialize()
    {
        int width, height;
        uint8_t *pPixels;
        auto &rIo = ImGui::GetIO();
        rIo.Fonts->GetTexDataAsRGBA32(&pPixels, &width, &height);

#if defined FASTCG_ANDROID
        // applying a hard-coded scale factor to ImGUI on Android!
        // TODO: make this less brittle and possibly dynamic
        const float SCALE_FACTOR = 3;
        rIo.FontGlobalScale = SCALE_FACTOR;
        auto &rStyle = ImGui::GetStyle();
        rStyle.ScaleAllSizes(SCALE_FACTOR);
#endif

        auto *pImGuiTexture = GraphicsSystem::GetInstance()->CreateTexture({"ImGui",
                                                                            (uint32_t)width,
                                                                            (uint32_t)height,
                                                                            1,
                                                                            1,
                                                                            TextureType::TEXTURE_2D,
                                                                            TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                                                                            TextureFormat::R8G8B8A8_UNORM,
                                                                            TextureFilter::LINEAR_FILTER,
                                                                            TextureWrapMode::CLAMP,
                                                                            pPixels});
        rIo.Fonts->SetTexID((void *)pImGuiTexture);

        mpImGuiShader = GraphicsSystem::GetInstance()->FindShader("ImGui");

        size_t verticesDataSize = MAX_NUM_VERTICES * sizeof(ImDrawVert);
        uint32_t indicesCount = MAX_NUM_VERTICES * 3;

        mpImGuiMesh = std::make_unique<Mesh>(MeshArgs{"ImGui Mesh",
                                                      {{"ImGui Mesh Vertices",
                                                        BufferUsageFlagBit::DYNAMIC,
                                                        verticesDataSize,
                                                        nullptr,
                                                        {{0, 2, VertexDataType::FLOAT, false, sizeof(ImDrawVert), offsetof(ImDrawVert, pos)},
                                                         {1, 2, VertexDataType::FLOAT, false, sizeof(ImDrawVert), offsetof(ImDrawVert, uv)},
                                                         {2, 4, VertexDataType::UNSIGNED_BYTE, true, sizeof(ImDrawVert), offsetof(ImDrawVert, col)}}}},
                                                      {BufferUsageFlagBit::DYNAMIC, indicesCount, nullptr}});

        mpImGuiConstantsBuffer = GraphicsSystem::GetInstance()->CreateBuffer({"ImGui Constants",
                                                                              BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC,
                                                                              sizeof(ImGuiConstants),
                                                                              &mImGuiConstants});

        mpVerticesData = std::make_unique<uint8_t[]>(verticesDataSize);
        mpIndicesData = std::make_unique<uint8_t[]>(indicesCount * sizeof(ImDrawIdx));
    }

    void ImGuiRenderer::Render(const ImDrawData *pImDrawData, GraphicsContext *pGraphicsContext)
    {
        pGraphicsContext->PushDebugMarker("ImGui Rendering");
        {
            pGraphicsContext->SetBlend(true);
            pGraphicsContext->SetBlendFunc(BlendFunc::ADD, BlendFunc::NONE);
            pGraphicsContext->SetBlendFactors(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ZERO, BlendFactor::ZERO);
            pGraphicsContext->SetCullMode(Face::NONE);
            pGraphicsContext->SetDepthTest(false);
            pGraphicsContext->SetDepthWrite(false);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetScissorTest(true);
            const auto *pBackbuffer = GraphicsSystem::GetInstance()->GetBackbuffer();
            pGraphicsContext->SetRenderTargets(&pBackbuffer, 1, nullptr);

            auto displayScale = pImDrawData->FramebufferScale;
            auto displayPos = ImVec2(pImDrawData->DisplayPos.x * displayScale.x, pImDrawData->DisplayPos.y * displayScale.y);
            auto displaySize = ImVec2(pImDrawData->DisplaySize.x * displayScale.x, pImDrawData->DisplaySize.y * displayScale.y);

            pGraphicsContext->SetViewport((int32_t)displayPos.x, (int32_t)displayPos.y, (uint32_t)displaySize.x, (uint32_t)displaySize.y);

            pGraphicsContext->PushDebugMarker("ImGui Passes");
            {
                mImGuiConstants.projection = glm::ortho(displayPos.x, displayPos.x + displaySize.x, displayPos.y + displaySize.y, displayPos.y);
                pGraphicsContext->Copy(mpImGuiConstantsBuffer, sizeof(ImGuiConstants), &mImGuiConstants);

                pGraphicsContext->BindShader(mpImGuiShader);

                pGraphicsContext->BindResource(mpImGuiConstantsBuffer, "ImGuiConstants");

                pGraphicsContext->SetVertexBuffers(mpImGuiMesh->GetVertexBuffers(), mpImGuiMesh->GetVertexBufferCount());
                pGraphicsContext->SetIndexBuffer(mpImGuiMesh->GetIndexBuffer());

                auto *pVerticesDataStart = mpVerticesData.get();
                auto *pVerticesDataEnd = pVerticesDataStart;
                auto *pIndicesDataStart = mpIndicesData.get();
                auto *pIndicesDataEnd = pIndicesDataStart;
                for (int n = 0; n < pImDrawData->CmdListsCount; n++)
                {
                    const auto *pCmdList = pImDrawData->CmdLists[n];
                    memcpy(pVerticesDataEnd, pCmdList->VtxBuffer.Data, pCmdList->VtxBuffer.size_in_bytes());
                    pVerticesDataEnd += pCmdList->VtxBuffer.size_in_bytes();
                    memcpy(pIndicesDataEnd, pCmdList->IdxBuffer.Data, pCmdList->IdxBuffer.size_in_bytes());
                    pIndicesDataEnd += pCmdList->IdxBuffer.size_in_bytes();
                }

                pGraphicsContext->Copy(mpImGuiMesh->GetVertexBuffers()[0], (pVerticesDataEnd - pVerticesDataStart), pVerticesDataStart);
                pGraphicsContext->Copy(mpImGuiMesh->GetIndexBuffer(), (pIndicesDataEnd - pIndicesDataStart), pIndicesDataStart);

                uint32_t idxOffset = 0;
                uint32_t vtxOffset = 0;
                for (int n = 0; n < pImDrawData->CmdListsCount; n++)
                {
                    const auto *pCmdList = pImDrawData->CmdLists[n];

                    for (int cmdIdx = 0; cmdIdx < pCmdList->CmdBuffer.Size; cmdIdx++)
                    {
                        const auto *pCmd = &pCmdList->CmdBuffer[cmdIdx];
                        if (pCmd->UserCallback)
                        {
                            pCmd->UserCallback(pCmdList, pCmd);
                        }
                        else
                        {
                            ImVec2 clipMin((pCmd->ClipRect.x - displayPos.x) * displayScale.x, (pCmd->ClipRect.y - displayPos.y) * displayScale.y);
                            ImVec2 clipMax((pCmd->ClipRect.z - displayPos.x) * displayScale.x, (pCmd->ClipRect.w - displayPos.y) * displayScale.y);
                            if (clipMin.x < 0.0f)
                            {
                                clipMin.x = 0.0f;
                            }
                            if (clipMin.y < 0.0f)
                            {
                                clipMin.y = 0.0f;
                            }
                            if (clipMax.x > displaySize.x)
                            {
                                clipMax.x = displaySize.x;
                            }
                            if (clipMax.y > displaySize.y)
                            {
                                clipMax.y = displaySize.y;
                            }
                            if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                            {
                                continue;
                            }

                            pGraphicsContext->SetScissor((int32_t)clipMin.x,
#if FASTCG_OPENGL
                                                         (int32_t)(displaySize.y - clipMax.y),
#else
                                                         (int32_t)clipMin.y,
#endif
                                                         (uint32_t)(clipMax.x - clipMin.x),
                                                         (uint32_t)(clipMax.y - clipMin.y));

                            pGraphicsContext->BindResource((Texture *)pCmd->GetTexID(), "uColorMap");

                            pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, pCmd->IdxOffset + idxOffset, (uint32_t)pCmd->ElemCount, (int32_t)pCmd->VtxOffset + vtxOffset);
                        }
                    }

                    idxOffset += pCmdList->IdxBuffer.Size;
                    vtxOffset += pCmdList->VtxBuffer.Size;
                }
            }
            pGraphicsContext->PopDebugMarker();
        }
        pGraphicsContext->PopDebugMarker();
    }

    void ImGuiRenderer::Finalize()
    {
        if (mpImGuiConstantsBuffer != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(mpImGuiConstantsBuffer);
            mpImGuiConstantsBuffer = nullptr;
        }

        mpImGuiMesh = nullptr;

        mpImGuiShader = nullptr;
    }
}