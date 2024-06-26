#include "Controls.h"

#include <FastCG/Core/Math.h>
#include <FastCG/Input/InputSystem.h>
#include <FastCG/Input/Key.h>
#include <FastCG/Platform/Application.h>
#include <FastCG/Rendering/Camera.h>
#include <FastCG/Rendering/RenderingSystem.h>
#include <FastCG/World/WorldSystem.h>

#include <imgui.h>

using namespace FastCG;

FASTCG_IMPLEMENT_COMPONENT(Controls, Behaviour);

void Controls::OnUpdate(float time, float deltaTime)
{
    if (InputSystem::GetKey(Key::ESCAPE))
    {
        Application::GetInstance()->Exit();
        return;
    }

    auto *pMainCamera = WorldSystem::GetInstance()->GetMainCamera();
    auto *pWorldRenderer = RenderingSystem::GetInstance()->GetWorldRenderer();

    if (ImGui::Begin("Controls"))
    {
        auto ssaoEnabled = pMainCamera->IsSSAOEnabled();
        ImGui::Checkbox("SSAO enabled (F1)", &ssaoEnabled);
        pMainCamera->SetSSAOEnabled(ssaoEnabled);
        auto ssaoBlurEnabled = pWorldRenderer->IsSSAOBlurEnabled();
        ImGui::Checkbox("SSAO blur enabled (F2)", &ssaoBlurEnabled);
        pWorldRenderer->SetSSAOBlurEnabled(ssaoBlurEnabled);
        auto ssaoRadius = pWorldRenderer->GetSSAORadius();
        ImGui::SliderFloat("SSAO radius (+/-)", &ssaoRadius, 0.01f, 1.0f);
        pWorldRenderer->SetSSAORadius(ssaoRadius);
    }
    ImGui::End();

    InputSystem::IsKeyPressed(Key::F1, mPressedKeyMask, 0,
                              [&]() { pMainCamera->SetSSAOEnabled(!pMainCamera->IsSSAOEnabled()); });
    InputSystem::IsKeyPressed(Key::F2, mPressedKeyMask, 2,
                              [&]() { pWorldRenderer->SetSSAOBlurEnabled(!pWorldRenderer->IsSSAOBlurEnabled()); });
    InputSystem::IsKeyPressed(Key::PLUS, mPressedKeyMask, 3, [&]() {
        pWorldRenderer->SetSSAORadius(MathF::Clamp(pWorldRenderer->GetSSAORadius() + 0.01f, 0.01f, 1));
    });
    InputSystem::IsKeyPressed(Key::MINUS, mPressedKeyMask, 4, [&]() {
        pWorldRenderer->SetSSAORadius(MathF::Clamp(pWorldRenderer->GetSSAORadius() - 0.01f, 0.01f, 1));
    });
}
