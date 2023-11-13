#ifndef FASTCG_LOG_H
#define FASTCG_LOG_H

#if _DEBUG

#include <FastCG/Core/Macros.h>

#include <stdio.h>

#if defined FASTCG_WINDOWS || defined FASTCG_LINUX
#define FASTCG_LOG_(severity, category, ...)                 \
    {                                                        \
        char msg[4096];                                      \
        snprintf(msg, FASTCG_ARRAYSIZE(msg), __VA_ARGS__);   \
        printf("[%s] [%s] %s\n", #category, #severity, msg); \
    }
#define FASTCG_LOG_VERBOSE(category, ...) FASTCG_LOG_(VERBOSE, category, __VA_ARGS__)
#define FASTCG_LOG_DEBUG(category, ...) FASTCG_LOG_(DEBUG, category, __VA_ARGS__)
#define FASTCG_LOG_INFO(category, ...) FASTCG_LOG_(INFO, category, __VA_ARGS__)
#define FASTCG_LOG_ERROR(category, ...) FASTCG_LOG_(ERROR, category, __VA_ARGS__)
#elif defined FASTCG_ANDROID
#include <android/log.h>
#define FASTCG_LOG_(category, severity, ...)                                              \
    {                                                                                     \
        char msg[4096];                                                                   \
        snprintf(msg, FASTCG_ARRAYSIZE(msg), __VA_ARGS__);                                \
        __android_log_print(ANDROID_LOG_##severity, "FASTCG", "[%s] %s", #category, msg); \
    }
#define FASTCG_LOG_VERBOSE(category, ...) FASTCG_LOG_(category, VERBOSE, __VA_ARGS__)
#define FASTCG_LOG_DEBUG(category, ...) FASTCG_LOG_(category, DEBUG, __VA_ARGS__)
#define FASTCG_LOG_INFO(category, ...) FASTCG_LOG_(category, INFO, __VA_ARGS__)
#define FASTCG_LOG_ERROR(category, ...) FASTCG_LOG_(category, ERROR, __VA_ARGS__)
#else
#error "FASTCG_LOG_*() macros are not implemented on the current platform"
#endif
#else
#define FASTCG_LOG_VERBOSE(category, ...)
#define FASTCG_LOG_DEBUG(category, ...)
#define FASTCG_LOG_INFO(category, ...)
#define FASTCG_LOG_ERROR(category, ...)
#endif

#endif