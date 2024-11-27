#ifndef FASTCG_OBJ_LOADER_H
#define FASTCG_OBJ_LOADER_H

#include <FastCG/Rendering/Material.h>
#include <FastCG/World/GameObject.h>

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>

namespace FastCG
{
    enum class OBJLoaderOption : uint8_t
    {
        NONE = 0,
        IS_SHADOW_CASTER = 1 << 0
    };

    using OBJLoaderOptionIntType = std::underlying_type<OBJLoaderOption>::type;
    using OBJLoaderOptionMaskType = uint8_t;

    class OBJLoader
    {
    public:
        static GameObject *Load(const std::string &rFileName, const std::shared_ptr<Material> &pDefaultMaterial,
                                OBJLoaderOptionMaskType options = (OBJLoaderOptionMaskType)OBJLoaderOption::NONE);

    private:
        OBJLoader() = delete;
        ~OBJLoader() = delete;
    };

}

#endif