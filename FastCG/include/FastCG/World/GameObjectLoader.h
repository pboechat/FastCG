#ifndef FASTCG_GAME_OBJECT_LOADER_H
#define FASTCG_GAME_OBJECT_LOADER_H

#include <FastCG/World/GameObject.h>

#include <cstdint>
#include <string>
#include <type_traits>

namespace FastCG
{
    enum class GameObjectLoaderOption : uint8_t
    {
        NONE = 0,
    };

    using GameObjectLoaderOptionIntType = std::underlying_type<GameObjectLoaderOption>::type;
    using GameObjectoaderOptionMaskType = uint8_t;

    class GameObjectLoader
    {
    public:
        static GameObject *Load(const std::string &rFilePath,
                                GameObjectoaderOptionMaskType options = (GameObjectoaderOptionMaskType)
                                    GameObjectLoaderOption::NONE);

    private:
        GameObjectLoader() = delete;
        ~GameObjectLoader() = delete;
    };

}

#endif