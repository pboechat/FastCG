#ifndef FASTCG_MODEL_LOADER_H
#define FASTCG_MODEL_LOADER_H

#include <FastCG/Rendering/Material.h>
#include <FastCG/World/GameObject.h>

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>

namespace FastCG
{
    enum class ModelLoaderOption : uint8_t
    {
        NONE = 0,
        IS_SHADOW_CASTER = 1 << 0
    };

    using ModelLoaderOptionIntType = std::underlying_type<ModelLoaderOption>::type;
    using ModelLoaderOptionMaskType = uint8_t;

    class ModelLoader
    {
    public:
        static GameObject *Load(const std::string &rFileName, const std::shared_ptr<Material> &pDefaultMaterial,
                                ModelLoaderOptionMaskType options = (ModelLoaderOptionMaskType)ModelLoaderOption::NONE);

    private:
        ModelLoader() = delete;
        ~ModelLoader() = delete;
    };

}

#endif