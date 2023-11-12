#ifndef FASTCG_LOG_H
#define FASTCG_LOG_H

#if defined FASTCG_WINDOWS || defined FASTCG_LINUX
// TODO: write proper log functions
#include <stdio.h>
#define FASTCG_LOG_VERBOSE(...) \
    printf(__VA_ARGS__);        \
    printf("\n")
#define FASTCG_LOG_DEBUG(...) \
    printf(__VA_ARGS__);      \
    printf("\n")
#define FASTCG_LOG_INFO(...) \
    printf(__VA_ARGS__);     \
    printf("\n")
#define FASTCG_LOG_ERROR(...) \
    printf(__VA_ARGS__);      \
    printf("\n")
#elif defined FASTCG_ANDROID
#include <android/log.h>
#define FASTCG_LOG_(severity, ...) __android_log_print(ANDROID_LOG_##severity, "FASTCG", __VA_ARGS__)
#define FASTCG_LOG_VERBOSE(...) FASTCG_LOG_(VERBOSE, __VA_ARGS__)
#define FASTCG_LOG_DEBUG(...) FASTCG_LOG_(DEBUG, __VA_ARGS__)
#define FASTCG_LOG_INFO(...) FASTCG_LOG_(INFO, __VA_ARGS__)
#define FASTCG_LOG_ERROR(...) FASTCG_LOG_(ERROR, __VA_ARGS__)
#else
#error "FASTCG_LOG_*() is not implemented on the current platform"
#endif

#endif