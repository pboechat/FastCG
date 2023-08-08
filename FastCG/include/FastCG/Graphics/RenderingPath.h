#ifndef FASTCG_RENDERING_PATH_H
#define FASTCG_RENDERING_PATH_H

#include <type_traits>
#include <cstdint>

namespace FastCG
{
    enum class RenderingPath : uint8_t
    {
        FORWARD,
        DEFERRED,
        LAST

    };

    using RenderingPathInt = std::underlying_type_t<RenderingPath>;
    using RenderingPathMask = uint8_t;

    constexpr const char *const FORWARD_RENDERING_PATH_PATTERN = "forward/";
    constexpr const char *const DEFERRED_RENDERING_PATH_PATTERN = "deferred/";
    constexpr const char *const RENDERING_PATH_PATH_PATTERNS[] = {FORWARD_RENDERING_PATH_PATTERN, DEFERRED_RENDERING_PATH_PATTERN};

}

#endif