#ifndef FASTCG_LOG_H
#define FASTCG_LOG_H

#include <FastCG/Core/Macros.h>

#if _DEBUG
#if defined FASTCG_WINDOWS || defined FASTCG_LINUX
#include <stdio.h>
#define FASTCG_LOG_(severity, category, ...)                   \
    {                                                          \
        char msg[4096];                                        \
        sprintf_s(msg, FASTCG_ARRAYSIZE(msg), __VA_ARGS__);    \
        printf_s("[%s] [%s] %s\n", #category, #severity, msg); \
    }
#define FASTCG_LOG_VERBOSE(category, ...) FASTCG_LOG_(VERBOSE, category, __VA_ARGS__)
#define FASTCG_LOG_DEBUG(category, ...) FASTCG_LOG_(DEBUG, category, __VA_ARGS__)
#define FASTCG_LOG_INFO(category, ...) FASTCG_LOG_(INFO, category, __VA_ARGS__)
#define FASTCG_LOG_ERROR(category, ...) FASTCG_LOG_(ERROR, category, __VA_ARGS__)
#elif defined FASTCG_ANDROID
#include <android/log.h>
#define FASTCG_LOG_(severity, ...) __android_log_print(ANDROID_LOG_##severity, "FASTCG", __VA_ARGS__)
#define FASTCG_LOG_VERBOSE(...) FASTCG_LOG_(VERBOSE, __VA_ARGS__)
#define FASTCG_LOG_DEBUG(...) FASTCG_LOG_(DEBUG, __VA_ARGS__)
#define FASTCG_LOG_INFO(...) FASTCG_LOG_(INFO, __VA_ARGS__)
#define FASTCG_LOG_ERROR(...) FASTCG_LOG_(ERROR, __VA_ARGS__)
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