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

    using DebugMenuItemCallback = std::function<void()>;

    class DebugMenuSystem
    {
        FASTCG_DECLARE_SYSTEM(DebugMenuSystem, DebugMenuSystemArgs);

    public:
        inline void AddItem(const std::string &name, const DebugMenuItemCallback &callback)
        {
            mItems.emplace(std::hash<std::string>()(name), Item{name, callback});
        }
        inline void RemoveItem(const std::string &name)
        {
            auto it = mItems.find(std::hash<std::string>()(name));
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

        DebugMenuSystem(const DebugMenuSystemArgs &rArgs) : mArgs(rArgs) {}

        void DrawMenu();

        friend class BaseApplication;
    };

}

#endif

#endif