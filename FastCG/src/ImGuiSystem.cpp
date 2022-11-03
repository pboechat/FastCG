#include <FastCG/ImGuiSystem.h>
#include <FastCG/InputSystem.h>

#include <imgui.h>

namespace FastCG
{
    void ImGuiSystem::Initialize()
    {
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    }

    void ImGuiSystem::BeginFrame(double deltaTime)
    {
        auto &io = ImGui::GetIO();
        io.DisplaySize.x = (float)mArgs.rScreenWidth;
        io.DisplaySize.y = (float)mArgs.rScreenHeight;
        io.DeltaTime = (float)deltaTime + 0.0000001f;
        auto mousePos = InputSystem::GetMousePosition();
        io.AddMousePosEvent((float)mousePos.x, (float)mousePos.y);
        io.AddMouseButtonEvent(0, InputSystem::GetMouseButton((MouseButton)0) == MouseButtonState::PRESSED);
        io.AddMouseButtonEvent(1, InputSystem::GetMouseButton((MouseButton)1) == MouseButtonState::PRESSED);
        io.AddMouseButtonEvent(2, InputSystem::GetMouseButton((MouseButton)2) == MouseButtonState::PRESSED);

        ImGui::NewFrame();
    }

    void ImGuiSystem::EndFrame()
    {
        ImGui::EndFrame();
        ImGui::Render();
    }

    void ImGuiSystem::Finalize()
    {
        ImGui::DestroyContext();
    }

}