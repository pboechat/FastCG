#ifndef FASTCG_DEBUG_MENU_SYSTEM_H
#define FASTCG_DEBUG_MENU_SYSTEM_H

#ifdef _DEBUG

#include <FastCG/System.h>

#include <unordered_map>
#include <string>
#include <functional>
#include <cstdint>
#include <cassert>

namespace FastCG
{
    class BaseApplication;

    struct DebugMenuSystemArgs
    {
    };

    using DebugMenuItemCallback = std::function<void(int &)>;
    using DebugMenuCallback = std::function<void(int)>;

    class DebugMenuSystem
    {
        FASTCG_DECLARE_SYSTEM(DebugMenuSystem, DebugMenuSystemArgs);

    public:
        inline void AddCallback(const std::string &rName, const DebugMenuCallback &rCallback)
        {
            mCallbacks.emplace(std::hash<std::string>()(rName), rCallback);
        }
        inline void RemoveCallback(const std::string &rName)
        {
            auto it = mCallbacks.find(std::hash<std::string>()(rName));
            assert(it != mCallbacks.end());
            mCallbacks.erase(it);
        }
        inline void AddItem(const std::string &rName, const DebugMenuItemCallback &rItemCallback)
        {
            mItems.emplace(std::hash<std::string>()(rName), Item{rName, rItemCallback});
        }
        inline void RemoveItem(const std::string &rName)
        {
            auto it = mItems.find(std::hash<std::string>()(rName));
            assert(it != mItems.end());
            mItems.erase(it);
        }

    private:
        struct Item
        {
            std::string name;
            DebugMenuItemCallback callback;
            bool enabled{false};
        };

        const DebugMenuSystemArgs mArgs;
        std::unordered_map<size_t, Item> mItems;
        std::unordered_map<size_t, DebugMenuCallback> mCallbacks;

        DebugMenuSystem(const DebugMenuSystemArgs &rArgs) : mArgs(rArgs) {}

        void DrawMenu();

        friend class BaseApplication;
    };

}

#endif

#endif