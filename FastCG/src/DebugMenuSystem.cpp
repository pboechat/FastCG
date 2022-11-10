#ifdef _DEBUG

#include <FastCG/DebugMenuSystem.h>

#include <imgui.h>

namespace FastCG
{
    void DebugMenuSystem::DrawMenu()
    {
        if (ImGui::BeginMainMenuBar())
        {
            for (auto it = mItems.begin(); it != mItems.end(); ++it)
            {
                auto &rItem = it->second;
                if (ImGui::BeginMenu(rItem.name.c_str()))
                {
                    rItem.callback();
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMainMenuBar();
        }
    }

}

#endif