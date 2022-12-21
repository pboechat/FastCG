#include "Controls.h"

#include <FastCG/WorldSystem.h>
#include <FastCG/InputSystem.h>
#include <FastCG/Application.h>

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

    auto *pWorldRenderer = WorldSystem::GetInstance()->GetWorldRenderer();

    if (ImGui::Begin("Controls"))
    {
        auto shadowMapBias = pWorldRenderer->GetShadowMapBias();
        ImGui::SliderFloat("Shadow Map Bias", &shadowMapBias, 0, 0.1f);
        auto uvScale = pWorldRenderer->GetPCSSUvScale();
        ImGui::SliderFloat("UV Scale", &uvScale, 0, 0.2f);
        pWorldRenderer->SetPCSSUvScale(uvScale);
        auto blockerSearchSamples = (int)pWorldRenderer->GetPCSSBlockerSearchSamples();
        ImGui::SliderInt("Blocker Search Samples (+/-)", &blockerSearchSamples, 1, 64);
        pWorldRenderer->SetPCSSBlockerSearchSamples((uint32_t)blockerSearchSamples);
        auto pcfSamples = (int)pWorldRenderer->GetPCSSPCFSamples();
        ImGui::SliderInt("PCF Samples (Home/End)", &pcfSamples, 1, 64);
        pWorldRenderer->SetPCSSPCFSamples((uint32_t)pcfSamples);
    }
    ImGui::End();
}
