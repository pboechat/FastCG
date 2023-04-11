#include <FastCG/ImGuiRenderer.h>

#include <glm/gtc/matrix_transform.hpp>

namespace FastCG
{
    static_assert(sizeof(ImDrawIdx) == sizeof(uint32_t), "Please configure ImGui to use 32-bit indices");

    constexpr size_t MAX_NUM_VERTICES = 30000;

    void ImGuiRenderer::Initialize()
    {
        int width, height;
        unsigned char *pixels;
        auto &io = ImGui::GetIO();
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        auto *pImGuiTexture = GraphicsSystem::GetInstance()->CreateTexture({"ImGui",
                                                                            (uint32_t)width,
                                                                            (uint32_t)height,
                                                                            TextureType::TEXTURE_2D,
                                                                            TextureFormat::RGBA,
                                                                            {8, 8, 8, 8},
                                                                            TextureDataType::UNSIGNED_CHAR,
                                                                            TextureFilter::LINEAR_FILTER,
                                                                            TextureWrapMode::CLAMP,
                                                                            false,
                                                                            pixels});
        io.Fonts->SetTexID((void *)pImGuiTexture);

        mpImGuiShader = GraphicsSystem::GetInstance()->FindShader("ImGui");

        mpImGuiMesh = std::make_unique<Mesh>(MeshArgs{"ImGui Mesh",
                                                      {{"Vertices",
                                                        BufferUsage::STREAM,
                                                        MAX_NUM_VERTICES * sizeof(ImDrawVert),
                                                        nullptr,
                                                        {{0, 2, VertexDataType::FLOAT, false, sizeof(ImDrawVert), offsetof(ImDrawVert, pos)},
                                                         {1, 2, VertexDataType::FLOAT, false, sizeof(ImDrawVert), offsetof(ImDrawVert, uv)},
                                                         {2, 4, VertexDataType::UNSIGNED_BYTE, true, sizeof(ImDrawVert), offsetof(ImDrawVert, col)}}}},
                                                      {BufferUsage::STREAM, MAX_NUM_VERTICES * 3 * sizeof(ImDrawIdx), nullptr}});

        mpImGuiConstantsBuffer = GraphicsSystem::GetInstance()->CreateBuffer({"ImGui Constants",
                                                                              BufferType::UNIFORM,
                                                                              BufferUsage::DYNAMIC,
                                                                              sizeof(ImGuiConstants),
                                                                              &mImGuiConstants});
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
            pGraphicsContext->SetRenderTargets(&pBackbuffer, 1);

            auto displayScale = pImDrawData->FramebufferScale;
            auto displayPos = ImVec2(pImDrawData->DisplayPos.x * displayScale.x, pImDrawData->DisplayPos.y * displayScale.y);
            auto displaySize = ImVec2(pImDrawData->DisplaySize.x * displayScale.x, pImDrawData->DisplaySize.y * displayScale.y);

            pGraphicsContext->SetViewport((int32_t)displayPos.x, (int32_t)displayPos.y, (uint32_t)displaySize.x, (uint32_t)displaySize.y);

            pGraphicsContext->PushDebugMarker("ImGui Passes");
            {
                mImGuiConstants.projection = glm::ortho(displayPos.x, displayPos.x + displaySize.x, displayPos.y + displaySize.y, displayPos.y);
                pGraphicsContext->Copy(mpImGuiConstantsBuffer, sizeof(ImGuiConstants), &mImGuiConstants);

                pGraphicsContext->BindShader(mpImGuiShader);

                pGraphicsContext->BindResource(mpImGuiConstantsBuffer, 0u);

                pGraphicsContext->SetVertexBuffers(mpImGuiMesh->GetVertexBuffers(), mpImGuiMesh->GetVertexBufferCount());
                pGraphicsContext->SetIndexBuffer(mpImGuiMesh->GetIndexBuffer());

                for (int n = 0; n < pImDrawData->CmdListsCount; n++)
                {
                    const auto *cmdList = pImDrawData->CmdLists[n];

                    pGraphicsContext->Copy(mpImGuiMesh->GetVertexBuffers()[0], cmdList->VtxBuffer.size_in_bytes(), cmdList->VtxBuffer.Data);
                    pGraphicsContext->Copy(mpImGuiMesh->GetIndexBuffer(), cmdList->IdxBuffer.size_in_bytes(), cmdList->IdxBuffer.Data);

                    for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; cmdIdx++)
                    {
                        const auto *pCmd = &cmdList->CmdBuffer[cmdIdx];
                        if (pCmd->UserCallback)
                        {
                            pCmd->UserCallback(cmdList, pCmd);
                        }
                        else
                        {
                            ImVec2 clipMin((pCmd->ClipRect.x - displayPos.x) * displayScale.x, (pCmd->ClipRect.y - displayPos.y) * displayScale.y);
                            ImVec2 clipMax((pCmd->ClipRect.z - displayPos.x) * displayScale.x, (pCmd->ClipRect.w - displayPos.y) * displayScale.y);
                            if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                            {
                                continue;
                            }

                            pGraphicsContext->SetScissor((int32_t)clipMin.x, (int32_t)(displaySize.y - clipMax.y), (uint32_t)(clipMax.x - clipMin.x), (uint32_t)(clipMax.y - clipMin.y));

                            pGraphicsContext->BindResource((Texture *)pCmd->GetTexID(), "uColorMap", 0);

                            pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, (uint32_t)(pCmd->IdxOffset * sizeof(ImDrawIdx)), (uint32_t)pCmd->ElemCount, (int32_t)pCmd->VtxOffset);
                        }
                    }
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