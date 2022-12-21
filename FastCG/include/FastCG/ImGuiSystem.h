#ifndef FASTCG_IMGUI_SYSTEM_H
#define FASTCG_IMGUI_SYSTEM_H

#include <FastCG/System.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/InputSystem.h>

#include <memory>
#include <cstdint>

namespace FastCG
{
    class ImGuiRenderer;

    struct ImGuiSystemArgs
    {
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
    };

    class ImGuiSystem
    {
        FASTCG_DECLARE_SYSTEM(ImGuiSystem, ImGuiSystemArgs);

    private:
        const ImGuiSystemArgs mArgs;
        std::unique_ptr<ImGuiRenderer> mpImGuiRenderer;
        RenderingContext *mpRenderingContext;

        ImGuiSystem(const ImGuiSystemArgs &rArgs);
        virtual ~ImGuiSystem();

        void Initialize();
        void BeginFrame(double deltaTime, KeyChange keyChanges[KEY_COUNT]);
        void EndFrame();
        void Render();
        void Finalize();
    };

}

#endif
