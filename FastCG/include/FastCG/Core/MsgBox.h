#ifndef FASTCG_MSG_BOX_H
#define FASTCG_MSG_BOX_H

#include <FastCG/Core/Macros.h>

#if defined FASTCG_WINDOWS
#define NOMINMAX
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <messagebox.h>
#include <wchar.h>
#endif

#include <stdio.h>

namespace FastCG
{
#if defined FASTCG_WINDOWS
#define FASTCG_MSG_BOX(title, fmt, ...)                                                                                \
    {                                                                                                                  \
        char __msgBoxBuffer[4096];                                                                                     \
        FASTCG_COMPILER_WARN_PUSH                                                                                      \
        FASTCG_COMPILER_WARN_IGNORE_FORMAT_TRUNCATION                                                                  \
        snprintf(__msgBoxBuffer, FASTCG_ARRAYSIZE(__msgBoxBuffer), fmt, ##__VA_ARGS__);                                \
        FASTCG_COMPILER_WARN_POP                                                                                       \
        MessageBoxA(NULL, __msgBoxBuffer, title, MB_ICONWARNING);                                                      \
    }
#elif defined FASTCG_LINUX
#define FASTCG_MSG_BOX(title, fmt, ...)                                                                                \
    {                                                                                                                  \
        wchar_t __wMsgBoxBuffer[4096];                                                                                 \
        wchar_t __wMsgBoxFmt[1024];                                                                                    \
        swprintf(__wMsgBoxFmt, FASTCG_ARRAYSIZE(__wMsgBoxFmt), L"%hs", fmt);                                           \
        swprintf(__wMsgBoxBuffer, FASTCG_ARRAYSIZE(__wMsgBoxBuffer), __wMsgBoxFmt, ##__VA_ARGS__);                     \
        wchar_t wButtonLabel[] = L"OK";                                                                                \
        Button button;                                                                                                 \
        button.label = wButtonLabel;                                                                                   \
        button.result = 0;                                                                                             \
        Messagebox(title, __wMsgBoxBuffer, &button, 1);                                                                \
    }
#elif defined FASTCG_ANDROID
    // TODO: implement a dialog box on Android - although I guess there's no easy way to do it
#define FASTCG_MSG_BOX(...)
#else
#error "FASTCG_MSG_BOX() macro is not implemented on the current platform"
#endif

}

#endif