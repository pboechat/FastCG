#ifndef FASTCG_H
#define FASTCG_H

#if defined FASTCG_WINDOWS
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <stdio.h>
#include <wchar.h>
#include <messagebox.h>
#include <signal.h>
#endif

#include <cstdio>

#define FASTCG_ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

#if defined FASTCG_WINDOWS
#define FASTCG_MSG_BOX(title, fmt, ...)                            \
    {                                                              \
        char msg[4096];                                            \
        sprintf_s(msg, FASTCG_ARRAYSIZE(msg), fmt, ##__VA_ARGS__); \
        MessageBoxA(NULL, msg, title, MB_ICONWARNING);             \
    }
#elif defined FASTCG_LINUX
#define FASTCG_MSG_BOX(title, fmt, ...)                              \
    {                                                                \
        wchar_t wMsg[4096];                                          \
        wchar_t wFmt[1024];                                          \
        swprintf(wFmt, FASTCG_ARRAYSIZE(wFmt), L"%hs", fmt);         \
        swprintf(wMsg, FASTCG_ARRAYSIZE(wMsg), wFmt, ##__VA_ARGS__); \
        wchar_t wButtonLabel[] = L"OK";                              \
        Button button;                                               \
        button.label = wButtonLabel;                                 \
        button.result = 0;                                           \
        Messagebox(title, wMsg, &button, 1);                         \
    }
#else
#error "FASTCG_MSG_BOX() is not implemented on the current platform"
#endif

#ifdef _DEBUG
#if defined FASTCG_WINDOWS
#define FASTCG_BREAK_TO_DEBUGGER() __debugbreak()
#elif defined FASTCG_LINUX
#define FASTCG_BREAK_TO_DEBUGGER() raise(SIGTRAP)
#else
#error "FASTCG_BREAK_TO_DEBUGGER() is not implemented on the current platform"
#endif
#else
#define FASTCG_BREAK_TO_DEBUGGER()
#endif

#if defined FASTCG_WINDOWS
#define FASTCG_WARN_PUSH _Pragma("warning(push)")
#define FASTCG_WARN_IGNORE_MACRO_ARGS _Pragma("warning(disable:4003)")
#define FASTCG_WARN_POP _Pragma("warning(pop)")
#elif defined FASTCG_LINUX
#define FASTCG_WARN_PUSH _Pragma("GCC diagnostic push")
#define FASTCG_WARN_IGNORE_MACRO_ARGS _Pragma("GCC diagnostic ignored \"-W\"")
#define FASTCG_WARN_POP _Pragma("GCC diagnostic pop")
#else
#error "FASTCG_WARN_* macros are not implemented on the current platform"
#endif

#endif