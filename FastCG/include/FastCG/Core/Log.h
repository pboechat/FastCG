#ifndef FASTCG_LOG_H
#define FASTCG_LOG_H

#define FASTCG_LOG_SEVERITY_VERBOSE 4
#define FASTCG_LOG_SEVERITY_DEBUG 3
#define FASTCG_LOG_SEVERITY_INFO 2
#define FASTCG_LOG_SEVERITY_WARN 1
#define FASTCG_LOG_SEVERITY_ERROR 0

#if _DEBUG

#if !defined FASTCG_LOG_SEVERITY
#define FASTCG_LOG_SEVERITY 3
#endif

#include <FastCG/Core/Macros.h>

#include <stdio.h>

#if defined FASTCG_WINDOWS || defined FASTCG_LINUX
#define FASTCG_LOG_(severity, category, ...)                                                                           \
    {                                                                                                                  \
        char __logBuffer[4096];                                                                                        \
        FASTCG_COMPILER_WARN_PUSH                                                                                      \
        FASTCG_COMPILER_WARN_IGNORE_FORMAT_TRUNCATION                                                                  \
        snprintf(__logBuffer, FASTCG_ARRAYSIZE(__logBuffer), __VA_ARGS__);                                             \
        FASTCG_COMPILER_WARN_POP                                                                                       \
        printf("[%s] [%s] %s\n", #category, #severity, __logBuffer);                                                   \
    }
#if FASTCG_LOG_SEVERITY >= FASTCG_LOG_SEVERITY_VERBOSE
#define FASTCG_LOG_VERBOSE(category, ...) FASTCG_LOG_(VERBOSE, category, __VA_ARGS__)
#else
#define FASTCG_LOG_VERBOSE(category, ...)
#endif
#if FASTCG_LOG_SEVERITY >= FASTCG_LOG_SEVERITY_DEBUG
#define FASTCG_LOG_DEBUG(category, ...) FASTCG_LOG_(DEBUG, category, __VA_ARGS__)
#else
#define FASTCG_LOG_DEBUG(category, ...)
#endif
#define FASTCG_LOG_INFO(category, ...) FASTCG_LOG_(INFO, category, __VA_ARGS__)
#define FASTCG_LOG_WARN(category, ...) FASTCG_LOG_(WARN, category, __VA_ARGS__)
#define FASTCG_LOG_ERROR(category, ...) FASTCG_LOG_(ERROR, category, __VA_ARGS__)
#elif defined FASTCG_ANDROID
#include <android/log.h>
#define FASTCG_LOG_(category, severity, ...)                                                                           \
    {                                                                                                                  \
        char __logBuffer[4096];                                                                                        \
        FASTCG_COMPILER_WARN_PUSH                                                                                      \
        FASTCG_COMPILER_WARN_IGNORE_FORMAT_TRUNCATION                                                                  \
        snprintf(__logBuffer, FASTCG_ARRAYSIZE(__logBuffer), __VA_ARGS__);                                             \
        FASTCG_COMPILER_WARN_POP                                                                                       \
        __android_log_print(ANDROID_LOG_##severity, "FASTCG", "[%s] %s", #category, __logBuffer);                      \
    }
#if FASTCG_LOG_SEVERITY >= FASTCG_LOG_SEVERITY_VERBOSE
#define FASTCG_LOG_VERBOSE(category, ...) FASTCG_LOG_(category, VERBOSE, __VA_ARGS__)
#else
#define FASTCG_LOG_VERBOSE(category, ...)
#endif
#if FASTCG_LOG_SEVERITY >= FASTCG_LOG_SEVERITY_DEBUG
#define FASTCG_LOG_DEBUG(category, ...) FASTCG_LOG_(category, DEBUG, __VA_ARGS__)
#else
#define FASTCG_LOG_DEBUG(category, ...)
#endif
#if FASTCG_LOG_SEVERITY >= FASTCG_LOG_SEVERITY_INFO
#define FASTCG_LOG_INFO(category, ...) FASTCG_LOG_(category, INFO, __VA_ARGS__)
#else
#define FASTCG_LOG_INFO(category, ...)
#endif
#if FASTCG_LOG_SEVERITY >= FASTCG_LOG_SEVERITY_WARN
#define FASTCG_LOG_WARN(category, ...) FASTCG_LOG_(category, WARN, __VA_ARGS__)
#else
#define FASTCG_LOG_WARN(category, ...)
#endif
#if FASTCG_LOG_SEVERITY >= FASTCG_LOG_SEVERITY_ERROR
#define FASTCG_LOG_ERROR(category, ...) FASTCG_LOG_(category, ERROR, __VA_ARGS__)
#else
#define FASTCG_LOG_ERROR(category, ...)
#endif
#else
#error "FASTCG_LOG_*() macros are not implemented on the current platform"
#endif
#else
#define FASTCG_LOG_VERBOSE(category, ...)
#define FASTCG_LOG_DEBUG(category, ...)
#define FASTCG_LOG_INFO(category, ...)
#define FASTCG_LOG_WARN(category, ...)
#define FASTCG_LOG_ERROR(category, ...)
#endif

#endif