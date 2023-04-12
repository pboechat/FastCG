#ifndef FASTCG_IMGUI_SYSTEM_H
#define FASTCG_IMGUI_SYSTEM_H

#include <FastCG/Input/InputSystem.h>
#include <FastCG/Core/System.h>

#include <cstdint>

namespace FastCG
{
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

        ImGuiSystem(const ImGuiSystemArgs &rArgs);
        ~ImGuiSystem();

        void Initialize();
        void BeginFrame(double deltaTime, KeyChange keyChanges[KEY_COUNT]);
        void EndFrame();
        void Finalize();
    };

}

#endif
