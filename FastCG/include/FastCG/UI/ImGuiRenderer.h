#ifndef FASTCG_IMGUI_RENDERER_H
#define FASTCG_IMGUI_RENDERER_H

#include <FastCG/Rendering/ShaderConstants.h>
#include <FastCG/Rendering/Mesh.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Core/Macros.h>

#include <imgui.h>

#include <memory>
#include <cstdint>

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
        ImGuiRenderer(const ImGuiRendererArgs &rArgs) { FASTCG_UNUSED(rArgs); }

        void Initialize();
        void Render(const ImDrawData *pImDrawData, GraphicsContext *pGraphicsContext);
        void Finalize();

    private:
        const Shader *mpImGuiShader{nullptr};
        const Buffer *mpImGuiConstantsBuffer{nullptr};
        const Texture *mpImGuiTexture{nullptr};
        std::unique_ptr<Mesh> mpImGuiMesh{nullptr};
        ImGuiConstants mImGuiConstants{};
        std::unique_ptr<uint8_t[]> mpVerticesData;
        std::unique_ptr<uint8_t[]> mpIndicesData;
    };

}

#endif