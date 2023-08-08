#ifndef FASTCG_MACROS_H
#define FASTCG_MACROS_H

#if defined FASTCG_WINDOWS
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <signal.h>
#endif

#define FASTCG_EXPAND(x) x

// source: https://gist.github.com/thwarted/8ce47e1897a578f4e80a
#define FASTCG_GET_LAST_ARG(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, ARG, ...) ARG
#define FASTCG_ARG_COUNT(...) FASTCG_ARG_COUNT_(##__VA_ARGS__, FASTCG_ARG_N())
#define FASTCG_ARG_COUNT_(...) FASTCG_GET_LAST_ARG(##__VA_ARGS__)
#define FASTCG_ARG_N() 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define FASTCG_FOR_EACH_ITER_(next, args) next args
#define FASTCG_FOR_EACH_ITER_02_(next, args) next args
#define FASTCG_FOR_EACH_ITER_03_(next, args) next args
#define FASTCG_FOR_EACH_ITER_04_(next, args) next args
#define FASTCG_FOR_EACH_ITER_04_(next, args) next args
#define FASTCG_FOR_EACH_ITER_05_(next, args) next args
#define FASTCG_FOR_EACH_ITER_06_(next, args) next args
#define FASTCG_FOR_EACH_ITER_07_(next, args) next args
#define FASTCG_FOR_EACH_ITER_08_(next, args) next args
#define FASTCG_FOR_EACH_ITER_09_(next, args) next args
#define FASTCG_FOR_EACH_ITER_10_(next, args) next args
#define FASTCG_FOR_EACH_ITER_11_(next, args) next args
#define FASTCG_FOR_EACH_ITER_12_(next, args) next args
#define FASTCG_FOR_EACH_ITER_13_(next, args) next args
#define FASTCG_FOR_EACH_ITER_14_(next, args) next args
#define FASTCG_FOR_EACH_ITER_15_(next, args) next args
#define FASTCG_FOR_EACH_ITER_16_(next, args) next args
#define FASTCG_FOR_EACH_ITER_17_(next, args) next args
#define FASTCG_FOR_EACH_ITER_18_(next, args) next args
#define FASTCG_FOR_EACH_ITER_19_(next, args) next args
#define FASTCG_FOR_EACH_ITER_20_(next, args) next args
#define FASTCG_FOR_EACH_ITER_21_(next, args) next args
#define FASTCG_FOR_EACH_ITER_00()
#define FASTCG_FOR_EACH_ITER_01(call, arg, ...) call(arg, 0)
#define FASTCG_FOR_EACH_ITER_02(call, arg, ...) call(arg, 1) FASTCG_FOR_EACH_ITER_02_(FASTCG_FOR_EACH_ITER_01, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_03(call, arg, ...) call(arg, 2) FASTCG_FOR_EACH_ITER_03_(FASTCG_FOR_EACH_ITER_02, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_04(call, arg, ...) call(arg, 3) FASTCG_FOR_EACH_ITER_04_(FASTCG_FOR_EACH_ITER_03, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_05(call, arg, ...) call(arg, 4) FASTCG_FOR_EACH_ITER_05_(FASTCG_FOR_EACH_ITER_04, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_06(call, arg, ...) call(arg, 5) FASTCG_FOR_EACH_ITER_06_(FASTCG_FOR_EACH_ITER_05, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_07(call, arg, ...) call(arg, 6) FASTCG_FOR_EACH_ITER_07_(FASTCG_FOR_EACH_ITER_06, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_08(call, arg, ...) call(arg, 7) FASTCG_FOR_EACH_ITER_08_(FASTCG_FOR_EACH_ITER_07, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_09(call, arg, ...) call(arg, 8) FASTCG_FOR_EACH_ITER_09_(FASTCG_FOR_EACH_ITER_08, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_10(call, arg, ...) call(arg, 9) FASTCG_FOR_EACH_ITER_10_(FASTCG_FOR_EACH_ITER_09, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_11(call, arg, ...) call(arg, 10) FASTCG_FOR_EACH_ITER_11_(FASTCG_FOR_EACH_ITER_10, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_12(call, arg, ...) call(arg, 11) FASTCG_FOR_EACH_ITER_12_(FASTCG_FOR_EACH_ITER_11, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_13(call, arg, ...) call(arg, 12) FASTCG_FOR_EACH_ITER_13_(FASTCG_FOR_EACH_ITER_12, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_14(call, arg, ...) call(arg, 13) FASTCG_FOR_EACH_ITER_14_(FASTCG_FOR_EACH_ITER_13, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_15(call, arg, ...) call(arg, 14) FASTCG_FOR_EACH_ITER_15_(FASTCG_FOR_EACH_ITER_14, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_16(call, arg, ...) call(arg, 15) FASTCG_FOR_EACH_ITER_16_(FASTCG_FOR_EACH_ITER_15, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_17(call, arg, ...) call(arg, 16) FASTCG_FOR_EACH_ITER_17_(FASTCG_FOR_EACH_ITER_16, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_18(call, arg, ...) call(arg, 17) FASTCG_FOR_EACH_ITER_18_(FASTCG_FOR_EACH_ITER_17, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_19(call, arg, ...) call(arg, 18) FASTCG_FOR_EACH_ITER_19_(FASTCG_FOR_EACH_ITER_18, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_20(call, arg, ...) call(arg, 19) FASTCG_FOR_EACH_ITER_20_(FASTCG_FOR_EACH_ITER_19, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_21(call, arg, ...) call(arg, 20) FASTCG_FOR_EACH_ITER_21_(FASTCG_FOR_EACH_ITER_20, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_22(...) \
    error:                           \
    FASTCG_FOR_EACH only supports up to 21 arguments
#define FASTCG_FOR_EACH(call, ...) FASTCG_FOR_EACH_ITER_(FASTCG_GET_LAST_ARG(_0,                                                                                                 \
                                                                             ##__VA_ARGS__,                                                                                      \
                                                                             FASTCG_FOR_EACH_ITER_22, FASTCG_FOR_EACH_ITER_21, FASTCG_FOR_EACH_ITER_20, FASTCG_FOR_EACH_ITER_19, \
                                                                             FASTCG_FOR_EACH_ITER_18, FASTCG_FOR_EACH_ITER_17, FASTCG_FOR_EACH_ITER_16, FASTCG_FOR_EACH_ITER_15, \
                                                                             FASTCG_FOR_EACH_ITER_14, FASTCG_FOR_EACH_ITER_13, FASTCG_FOR_EACH_ITER_12, FASTCG_FOR_EACH_ITER_11, \
                                                                             FASTCG_FOR_EACH_ITER_10, FASTCG_FOR_EACH_ITER_09, FASTCG_FOR_EACH_ITER_08, FASTCG_FOR_EACH_ITER_07, \
                                                                             FASTCG_FOR_EACH_ITER_06, FASTCG_FOR_EACH_ITER_05, FASTCG_FOR_EACH_ITER_04, FASTCG_FOR_EACH_ITER_03, \
                                                                             FASTCG_FOR_EACH_ITER_02, FASTCG_FOR_EACH_ITER_01, FASTCG_FOR_EACH_ITER_00),                         \
                                                         (call, ##__VA_ARGS__))

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