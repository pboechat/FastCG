#ifndef FASTCG_MACROS_H
#define FASTCG_MACROS_H

#if defined FASTCG_WINDOWS
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <signal.h>
#endif

// source: https://gist.github.com/thwarted/8ce47e1897a578f4e80a
#define FASTCG_FE_CALLITn01(a, b) a b
#define FASTCG_FE_CALLITn02(a, b) a b
#define FASTCG_FE_CALLITn03(a, b) a b
#define FASTCG_FE_CALLITn04(a, b) a b
#define FASTCG_FE_CALLITn04(a, b) a b
#define FASTCG_FE_CALLITn05(a, b) a b
#define FASTCG_FE_CALLITn06(a, b) a b
#define FASTCG_FE_CALLITn07(a, b) a b
#define FASTCG_FE_CALLITn08(a, b) a b
#define FASTCG_FE_CALLITn09(a, b) a b
#define FASTCG_FE_CALLITn10(a, b) a b
#define FASTCG_FE_CALLITn11(a, b) a b
#define FASTCG_FE_CALLITn12(a, b) a b
#define FASTCG_FE_CALLITn13(a, b) a b
#define FASTCG_FE_CALLITn14(a, b) a b
#define FASTCG_FE_CALLITn15(a, b) a b
#define FASTCG_FE_CALLITn16(a, b) a b
#define FASTCG_FE_CALLITn17(a, b) a b
#define FASTCG_FE_CALLITn18(a, b) a b
#define FASTCG_FE_CALLITn19(a, b) a b
#define FASTCG_FE_CALLITn20(a, b) a b
#define FASTCG_FE_CALLITn21(a, b) a b
#define FASTCG_FE_n00()
#define FASTCG_FE_n01(what, a, ...) what(a)
#define FASTCG_FE_n02(what, a, ...) what(a) FASTCG_FE_CALLITn02(FASTCG_FE_n01, (what, ##__VA_ARGS__))
#define FASTCG_FE_n03(what, a, ...) what(a) FASTCG_FE_CALLITn03(FASTCG_FE_n02, (what, ##__VA_ARGS__))
#define FASTCG_FE_n04(what, a, ...) what(a) FASTCG_FE_CALLITn04(FASTCG_FE_n03, (what, ##__VA_ARGS__))
#define FASTCG_FE_n05(what, a, ...) what(a) FASTCG_FE_CALLITn05(FASTCG_FE_n04, (what, ##__VA_ARGS__))
#define FASTCG_FE_n06(what, a, ...) what(a) FASTCG_FE_CALLITn06(FASTCG_FE_n05, (what, ##__VA_ARGS__))
#define FASTCG_FE_n07(what, a, ...) what(a) FASTCG_FE_CALLITn07(FASTCG_FE_n06, (what, ##__VA_ARGS__))
#define FASTCG_FE_n08(what, a, ...) what(a) FASTCG_FE_CALLITn08(FASTCG_FE_n07, (what, ##__VA_ARGS__))
#define FASTCG_FE_n09(what, a, ...) what(a) FASTCG_FE_CALLITn09(FASTCG_FE_n08, (what, ##__VA_ARGS__))
#define FASTCG_FE_n10(what, a, ...) what(a) FASTCG_FE_CALLITn10(FASTCG_FE_n09, (what, ##__VA_ARGS__))
#define FASTCG_FE_n11(what, a, ...) what(a) FASTCG_FE_CALLITn11(FASTCG_FE_n10, (what, ##__VA_ARGS__))
#define FASTCG_FE_n12(what, a, ...) what(a) FASTCG_FE_CALLITn12(FASTCG_FE_n11, (what, ##__VA_ARGS__))
#define FASTCG_FE_n13(what, a, ...) what(a) FASTCG_FE_CALLITn13(FASTCG_FE_n12, (what, ##__VA_ARGS__))
#define FASTCG_FE_n14(what, a, ...) what(a) FASTCG_FE_CALLITn14(FASTCG_FE_n13, (what, ##__VA_ARGS__))
#define FASTCG_FE_n15(what, a, ...) what(a) FASTCG_FE_CALLITn15(FASTCG_FE_n14, (what, ##__VA_ARGS__))
#define FASTCG_FE_n16(what, a, ...) what(a) FASTCG_FE_CALLITn16(FASTCG_FE_n15, (what, ##__VA_ARGS__))
#define FASTCG_FE_n17(what, a, ...) what(a) FASTCG_FE_CALLITn17(FASTCG_FE_n16, (what, ##__VA_ARGS__))
#define FASTCG_FE_n18(what, a, ...) what(a) FASTCG_FE_CALLITn18(FASTCG_FE_n17, (what, ##__VA_ARGS__))
#define FASTCG_FE_n19(what, a, ...) what(a) FASTCG_FE_CALLITn19(FASTCG_FE_n18, (what, ##__VA_ARGS__))
#define FASTCG_FE_n20(what, a, ...) what(a) FASTCG_FE_CALLITn20(FASTCG_FE_n19, (what, ##__VA_ARGS__))
#define FASTCG_FE_n21(what, a, ...) what(a) FASTCG_FE_CALLITn21(FASTCG_FE_n20, (what, ##__VA_ARGS__))
#define FASTCG_FE_n22(...) \
    ERROR:                 \
    FOR_EACH only supports up to 21 arguments
#define FASTCG_FE_GET_MACRO(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, NAME, ...) NAME
#define FASTCG_FOR_EACH(what, ...) FASTCG_FE_CALLITn01(FASTCG_FE_GET_MACRO(_0, ##__VA_ARGS__, FASTCG_FE_n22, FASTCG_FE_n21, FASTCG_FE_n20, FASTCG_FE_n19,                                                                        \
                                                                           FASTCG_FE_n18, FASTCG_FE_n17, FASTCG_FE_n16, FASTCG_FE_n15, FASTCG_FE_n14, FASTCG_FE_n13, FASTCG_FE_n12, FASTCG_FE_n11, FASTCG_FE_n10, FASTCG_FE_n09, \
                                                                           FASTCG_FE_n08, FASTCG_FE_n07, FASTCG_FE_n06, FASTCG_FE_n05, FASTCG_FE_n04, FASTCG_FE_n03, FASTCG_FE_n02, FASTCG_FE_n01, FASTCG_FE_n00),               \
                                                       (what, ##__VA_ARGS__))

#define FASTCG_ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

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