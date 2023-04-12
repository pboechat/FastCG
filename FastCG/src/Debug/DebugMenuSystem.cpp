#ifdef _DEBUG

#include <FastCG/Debug/DebugMenuSystem.h>

#include <imgui.h>

namespace FastCG
{
    void DebugMenuSystem::DrawMenu()
    {
        int result = 0;
        if (ImGui::BeginMainMenuBar())
        {
            for (auto it = mItems.begin(); it != mItems.end(); ++it)
            {
                auto &rItem = it->second;
                if (ImGui::BeginMenu(rItem.name.c_str()))
                {
                    rItem.callback(result);
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMainMenuBar();
        }

        for (auto it = mCallbacks.begin(); it != mCallbacks.end(); ++it)
        {
            auto &rCallback = it->second;
            rCallback(result);
        }
    }

}

#endif