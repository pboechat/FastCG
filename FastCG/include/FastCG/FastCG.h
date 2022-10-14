#ifndef FASTCG_H
#define FASTCG_H

#ifdef _WIN32
#define FASTCG_WINDOWS
#endif

#ifdef FASTCG_WINDOWS
#include <Windows.h>
#endif

#include <cstdio>

#define FASTCG_ARRAYSIZE(x) sizeof(x) / sizeof(x[0])

#ifdef FASTCG_WINDOWS
#define FASTCG_MSG_BOX(title, fmt, ...)                            \
    {                                                              \
        char msg[1024];                                            \
        sprintf_s(msg, FASTCG_ARRAYSIZE(msg), fmt, ##__VA_ARGS__); \
        MessageBoxA(NULL, msg, title, MB_ICONWARNING);             \
    }
#else
#error "FASTCG_MSG_BOX() is not implemented on the current platform"
#endif

#ifdef _DEBUG
#ifdef FASTCG_WINDOWS
#define FASTCG_BREAK_TO_DEBUGGER() __debugbreak()
#else
#error "FASTCG_BREAK_TO_DEBUGGER() is not implemented on the current platform"
#endif
#else
#define FASTCG_BREAK_TO_DEBUGGER()
#endif

#endif