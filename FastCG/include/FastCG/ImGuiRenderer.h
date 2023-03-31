#ifndef FASTCG_IMGUI_RENDERER_H
#define FASTCG_IMGUI_RENDERER_H

#include <FastCG/ShaderConstants.h>
#include <FastCG/Graphics/GraphicsSystem.h>

#include <imgui.h>

#include <array>

namespace FastCG
{
    struct ImGuiRendererArgs
    {
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
    };

    class ImGuiRenderer
    {
    public:
        ImGuiRenderer(const ImGuiRendererArgs &rArgs) : mArgs(rArgs) {}

        void Initialize();
        void Render(const ImDrawData *pImDrawData, GraphicsContext *pGraphicsContext);
        void Finalize();

    private:
        const ImGuiRendererArgs mArgs;
        const Shader *mpImGuiShader{nullptr};
        const Mesh *mpImGuiMesh{nullptr};
        const Buffer *mpImGuiConstantsBuffer{nullptr};
        ImGuiConstants mImGuiConstants{};
    };

}

#endif