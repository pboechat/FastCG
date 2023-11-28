#ifndef FASTCG_RENDERING_UTILS_H
#define FASTCG_RENDERING_UTILS_H

#include <limits>
#include <cstdint>

namespace FastCG
{
    using RenderGroupInt = uint8_t;
    enum class RenderGroup : uint8_t
    {
        SHADOW_CASTERS = 0,
        SKYBOX = 1,
        RESERVED = 2,
        OPAQUE_MATERIAL = 3,
        TRANSPARENT_MATERIAL = (std::numeric_limits<RenderGroupInt>::max() / 2) + OPAQUE_MATERIAL - 1,
        LAST = std::numeric_limits<RenderGroupInt>::max()
    };

    constexpr RenderGroupInt OPAQUE_MATERIAL_ORDER_SPACE = (RenderGroupInt)RenderGroup::TRANSPARENT_MATERIAL - (RenderGroupInt)RenderGroup::OPAQUE_MATERIAL;
    constexpr RenderGroupInt TRANSPARENT_MATERIAL_ORDER_SPACE = (RenderGroupInt)RenderGroup::LAST - (RenderGroupInt)RenderGroup::TRANSPARENT_MATERIAL;
    static_assert(OPAQUE_MATERIAL_ORDER_SPACE == TRANSPARENT_MATERIAL_ORDER_SPACE);
    constexpr RenderGroupInt MATERIAL_ORDER_USER_SPACE = OPAQUE_MATERIAL_ORDER_SPACE;

    inline bool IsMaterialRenderGroup(RenderGroup group)
    {
        return group == RenderGroup::OPAQUE_MATERIAL || group == RenderGroup::TRANSPARENT_MATERIAL;
    }

}

#endif