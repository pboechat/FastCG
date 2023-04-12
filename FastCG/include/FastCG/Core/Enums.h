#ifndef FASTCG_ENUMS_H
#define FASTCG_ENUMS_H

#include <FastCG/Core/Macros.h>

#define FASTCG_EXPAND_TO_ENUM_STRING(x) "::" #x,

#define FASTCG_DECLARE_ENUM(enumName, type, ...)                                                                               \
    enum class enumName : type                                                                                                 \
    {                                                                                                                          \
        __VA_ARGS__,                                                                                                           \
        COUNT                                                                                                                  \
    };                                                                                                                         \
    constexpr const char *const enumName##_STRINGS[] = {FASTCG_FOR_EACH(#enumName FASTCG_EXPAND_TO_ENUM_STRING, __VA_ARGS__)}; \
    inline constexpr const char *Get##enumName##String(enumName value)                                                         \
    {                                                                                                                          \
        return enumName##_STRINGS[(type)value];                                                                                \
    }

#endif