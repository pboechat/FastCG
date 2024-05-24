#ifndef FASTCG_ENUMS_H
#define FASTCG_ENUMS_H

#include <FastCG/Core/Macros.h>

#define FASTCG_EXPAND_TO_ENUM_STRING(item, idx) "::" #item,

#define FASTCG_DECLARE_ENUM_FUNCTIONS_(enumName, type, ...)                                                            \
    constexpr const char *const enumName##_STRINGS[] = {                                                               \
        FASTCG_FOR_EACH(#enumName FASTCG_EXPAND_TO_ENUM_STRING, __VA_ARGS__)};                                         \
    inline constexpr const char *Get##enumName##String(enumName value)                                                 \
    {                                                                                                                  \
        return enumName##_STRINGS[(type)value];                                                                        \
    }

#define FASTCG_DECLARE_SCOPED_ENUM_(fn, enumName, type, ...)                                                           \
    enum class enumName : type                                                                                         \
    {                                                                                                                  \
        FASTCG_FOR_EACH(fn, __VA_ARGS__) LAST                                                                          \
    }

#define FASTCG_DECLARE_UNSCOPED_ENUM_(fn, enumName, ...)                                                               \
    enum enumName                                                                                                      \
    {                                                                                                                  \
        FASTCG_FOR_EACH(fn, __VA_ARGS__)                                                                               \
    }

#define FASTCG_EXPAND_TO_ENUM_ITEM(item, idx) item,

#define FASTCG_DECLARE_SCOPED_ENUM(enumName, type, ...)                                                                \
    FASTCG_DECLARE_SCOPED_ENUM_(FASTCG_EXPAND_TO_ENUM_ITEM, enumName, type, __VA_ARGS__);                              \
    FASTCG_DECLARE_ENUM_FUNCTIONS_(enumName, type, __VA_ARGS__)

#define FASTCG_DECLARE_UNSCOPED_ENUM(enumName, ...)                                                                    \
    FASTCG_DECLARE_UNSCOPED_ENUM_(FASTCG_EXPAND_TO_ENUM_ITEM, enumName, __VA_ARGS__);                                  \
    FASTCG_DECLARE_ENUM_FUNCTIONS_(enumName, int, __VA_ARGS__)

#define FASTCG_EXPAND_TO_FLAG_BIT_ITEM(item, idx) item = 1 << idx,

#define FASTCG_DECLARE_FLAGS(flagName, type, ...)                                                                      \
    using flagName##Flags = type;                                                                                      \
    FASTCG_DECLARE_UNSCOPED_ENUM_(FASTCG_EXPAND_TO_FLAG_BIT_ITEM, flagName##FlagBit, __VA_ARGS__)

#endif