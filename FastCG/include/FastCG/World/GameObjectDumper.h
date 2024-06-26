#ifndef FASTCG_GAME_OBJECT_DUMPER_H
#define FASTCG_GAME_OBJECT_DUMPER_H

#include <FastCG/World/GameObject.h>

#include <cstdint>
#include <string>
#include <type_traits>

namespace FastCG
{
    enum class GameObjectDumperOption : uint8_t
    {
        NONE = 0,
        ENCODE_DATA = 1,
    };

    using GameObjectDumperOptionIntType = std::underlying_type<GameObjectDumperOption>::type;
    using GameObjectDumperOptionMaskType = uint8_t;

    class GameObjectDumper
    {
    public:
        static void Dump(const std::string &rFilePath, GameObject *pWorld,
                         GameObjectDumperOptionMaskType options = (GameObjectDumperOptionMaskType)
                             GameObjectDumperOption::NONE);

    private:
        GameObjectDumper() = delete;
        ~GameObjectDumper() = delete;
    };

}

#endif