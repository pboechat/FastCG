#ifndef FASTCG_MACROS_H
#define FASTCG_MACROS_H

#define FASTCG_EXPAND(x) x

// source: https://gist.github.com/thwarted/8ce47e1897a578f4e80a
#define FASTCG_GET_LAST_ARG(_00, _01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17,  \
                            _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35,  \
                            _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53,  \
                            _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, ARG, ...)                      \
    ARG
#define FASTCG_ARG_COUNT(...) FASTCG_ARG_COUNT_(##__VA_ARGS__, FASTCG_ARG_N())
#define FASTCG_ARG_COUNT_(...) FASTCG_GET_LAST_ARG(##__VA_ARGS__)
#define FASTCG_ARG_N()                                                                                                 \
    65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38,    \
        37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,    \
        10, 09, 08, 07, 06, 05, 04, 03, 02, 01, 00
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
#define FASTCG_FOR_EACH_ITER_22_(next, args) next args
#define FASTCG_FOR_EACH_ITER_23_(next, args) next args
#define FASTCG_FOR_EACH_ITER_24_(next, args) next args
#define FASTCG_FOR_EACH_ITER_25_(next, args) next args
#define FASTCG_FOR_EACH_ITER_26_(next, args) next args
#define FASTCG_FOR_EACH_ITER_27_(next, args) next args
#define FASTCG_FOR_EACH_ITER_28_(next, args) next args
#define FASTCG_FOR_EACH_ITER_29_(next, args) next args
#define FASTCG_FOR_EACH_ITER_30_(next, args) next args
#define FASTCG_FOR_EACH_ITER_31_(next, args) next args
#define FASTCG_FOR_EACH_ITER_32_(next, args) next args
#define FASTCG_FOR_EACH_ITER_33_(next, args) next args
#define FASTCG_FOR_EACH_ITER_34_(next, args) next args
#define FASTCG_FOR_EACH_ITER_35_(next, args) next args
#define FASTCG_FOR_EACH_ITER_36_(next, args) next args
#define FASTCG_FOR_EACH_ITER_37_(next, args) next args
#define FASTCG_FOR_EACH_ITER_38_(next, args) next args
#define FASTCG_FOR_EACH_ITER_39_(next, args) next args
#define FASTCG_FOR_EACH_ITER_40_(next, args) next args
#define FASTCG_FOR_EACH_ITER_41_(next, args) next args
#define FASTCG_FOR_EACH_ITER_42_(next, args) next args
#define FASTCG_FOR_EACH_ITER_43_(next, args) next args
#define FASTCG_FOR_EACH_ITER_44_(next, args) next args
#define FASTCG_FOR_EACH_ITER_45_(next, args) next args
#define FASTCG_FOR_EACH_ITER_46_(next, args) next args
#define FASTCG_FOR_EACH_ITER_47_(next, args) next args
#define FASTCG_FOR_EACH_ITER_48_(next, args) next args
#define FASTCG_FOR_EACH_ITER_49_(next, args) next args
#define FASTCG_FOR_EACH_ITER_50_(next, args) next args
#define FASTCG_FOR_EACH_ITER_51_(next, args) next args
#define FASTCG_FOR_EACH_ITER_52_(next, args) next args
#define FASTCG_FOR_EACH_ITER_53_(next, args) next args
#define FASTCG_FOR_EACH_ITER_54_(next, args) next args
#define FASTCG_FOR_EACH_ITER_55_(next, args) next args
#define FASTCG_FOR_EACH_ITER_56_(next, args) next args
#define FASTCG_FOR_EACH_ITER_57_(next, args) next args
#define FASTCG_FOR_EACH_ITER_58_(next, args) next args
#define FASTCG_FOR_EACH_ITER_59_(next, args) next args
#define FASTCG_FOR_EACH_ITER_60_(next, args) next args
#define FASTCG_FOR_EACH_ITER_61_(next, args) next args
#define FASTCG_FOR_EACH_ITER_62_(next, args) next args
#define FASTCG_FOR_EACH_ITER_63_(next, args) next args
#define FASTCG_FOR_EACH_ITER_64_(next, args) next args
#define FASTCG_FOR_EACH_ITER_65_(next, args) next args
#define FASTCG_FOR_EACH_ITER_00()
#define FASTCG_FOR_EACH_ITER_01(call, arg, ...) call(arg, 0)
#define FASTCG_FOR_EACH_ITER_02(call, arg, ...)                                                                        \
    call(arg, 1) FASTCG_FOR_EACH_ITER_02_(FASTCG_FOR_EACH_ITER_01, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_03(call, arg, ...)                                                                        \
    call(arg, 2) FASTCG_FOR_EACH_ITER_03_(FASTCG_FOR_EACH_ITER_02, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_04(call, arg, ...)                                                                        \
    call(arg, 3) FASTCG_FOR_EACH_ITER_04_(FASTCG_FOR_EACH_ITER_03, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_05(call, arg, ...)                                                                        \
    call(arg, 4) FASTCG_FOR_EACH_ITER_05_(FASTCG_FOR_EACH_ITER_04, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_06(call, arg, ...)                                                                        \
    call(arg, 5) FASTCG_FOR_EACH_ITER_06_(FASTCG_FOR_EACH_ITER_05, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_07(call, arg, ...)                                                                        \
    call(arg, 6) FASTCG_FOR_EACH_ITER_07_(FASTCG_FOR_EACH_ITER_06, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_08(call, arg, ...)                                                                        \
    call(arg, 7) FASTCG_FOR_EACH_ITER_08_(FASTCG_FOR_EACH_ITER_07, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_09(call, arg, ...)                                                                        \
    call(arg, 8) FASTCG_FOR_EACH_ITER_09_(FASTCG_FOR_EACH_ITER_08, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_10(call, arg, ...)                                                                        \
    call(arg, 9) FASTCG_FOR_EACH_ITER_10_(FASTCG_FOR_EACH_ITER_09, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_11(call, arg, ...)                                                                        \
    call(arg, 10) FASTCG_FOR_EACH_ITER_11_(FASTCG_FOR_EACH_ITER_10, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_12(call, arg, ...)                                                                        \
    call(arg, 11) FASTCG_FOR_EACH_ITER_12_(FASTCG_FOR_EACH_ITER_11, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_13(call, arg, ...)                                                                        \
    call(arg, 12) FASTCG_FOR_EACH_ITER_13_(FASTCG_FOR_EACH_ITER_12, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_14(call, arg, ...)                                                                        \
    call(arg, 13) FASTCG_FOR_EACH_ITER_14_(FASTCG_FOR_EACH_ITER_13, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_15(call, arg, ...)                                                                        \
    call(arg, 14) FASTCG_FOR_EACH_ITER_15_(FASTCG_FOR_EACH_ITER_14, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_16(call, arg, ...)                                                                        \
    call(arg, 15) FASTCG_FOR_EACH_ITER_16_(FASTCG_FOR_EACH_ITER_15, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_17(call, arg, ...)                                                                        \
    call(arg, 16) FASTCG_FOR_EACH_ITER_17_(FASTCG_FOR_EACH_ITER_16, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_18(call, arg, ...)                                                                        \
    call(arg, 17) FASTCG_FOR_EACH_ITER_18_(FASTCG_FOR_EACH_ITER_17, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_19(call, arg, ...)                                                                        \
    call(arg, 18) FASTCG_FOR_EACH_ITER_19_(FASTCG_FOR_EACH_ITER_18, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_20(call, arg, ...)                                                                        \
    call(arg, 19) FASTCG_FOR_EACH_ITER_20_(FASTCG_FOR_EACH_ITER_19, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_21(call, arg, ...)                                                                        \
    call(arg, 20) FASTCG_FOR_EACH_ITER_21_(FASTCG_FOR_EACH_ITER_20, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_22(call, arg, ...)                                                                        \
    call(arg, 21) FASTCG_FOR_EACH_ITER_22_(FASTCG_FOR_EACH_ITER_21, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_23(call, arg, ...)                                                                        \
    call(arg, 22) FASTCG_FOR_EACH_ITER_23_(FASTCG_FOR_EACH_ITER_22, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_24(call, arg, ...)                                                                        \
    call(arg, 23) FASTCG_FOR_EACH_ITER_24_(FASTCG_FOR_EACH_ITER_23, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_25(call, arg, ...)                                                                        \
    call(arg, 24) FASTCG_FOR_EACH_ITER_25_(FASTCG_FOR_EACH_ITER_24, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_26(call, arg, ...)                                                                        \
    call(arg, 25) FASTCG_FOR_EACH_ITER_26_(FASTCG_FOR_EACH_ITER_25, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_27(call, arg, ...)                                                                        \
    call(arg, 26) FASTCG_FOR_EACH_ITER_27_(FASTCG_FOR_EACH_ITER_26, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_28(call, arg, ...)                                                                        \
    call(arg, 27) FASTCG_FOR_EACH_ITER_28_(FASTCG_FOR_EACH_ITER_27, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_29(call, arg, ...)                                                                        \
    call(arg, 28) FASTCG_FOR_EACH_ITER_29_(FASTCG_FOR_EACH_ITER_28, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_30(call, arg, ...)                                                                        \
    call(arg, 29) FASTCG_FOR_EACH_ITER_30_(FASTCG_FOR_EACH_ITER_29, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_31(call, arg, ...)                                                                        \
    call(arg, 30) FASTCG_FOR_EACH_ITER_31_(FASTCG_FOR_EACH_ITER_30, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_32(call, arg, ...)                                                                        \
    call(arg, 31) FASTCG_FOR_EACH_ITER_32_(FASTCG_FOR_EACH_ITER_31, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_33(call, arg, ...)                                                                        \
    call(arg, 32) FASTCG_FOR_EACH_ITER_33_(FASTCG_FOR_EACH_ITER_32, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_34(call, arg, ...)                                                                        \
    call(arg, 33) FASTCG_FOR_EACH_ITER_34_(FASTCG_FOR_EACH_ITER_33, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_35(call, arg, ...)                                                                        \
    call(arg, 34) FASTCG_FOR_EACH_ITER_35_(FASTCG_FOR_EACH_ITER_34, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_36(call, arg, ...)                                                                        \
    call(arg, 35) FASTCG_FOR_EACH_ITER_36_(FASTCG_FOR_EACH_ITER_35, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_37(call, arg, ...)                                                                        \
    call(arg, 36) FASTCG_FOR_EACH_ITER_37_(FASTCG_FOR_EACH_ITER_36, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_38(call, arg, ...)                                                                        \
    call(arg, 37) FASTCG_FOR_EACH_ITER_38_(FASTCG_FOR_EACH_ITER_37, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_39(call, arg, ...)                                                                        \
    call(arg, 38) FASTCG_FOR_EACH_ITER_39_(FASTCG_FOR_EACH_ITER_38, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_40(call, arg, ...)                                                                        \
    call(arg, 39) FASTCG_FOR_EACH_ITER_40_(FASTCG_FOR_EACH_ITER_39, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_41(call, arg, ...)                                                                        \
    call(arg, 40) FASTCG_FOR_EACH_ITER_41_(FASTCG_FOR_EACH_ITER_40, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_42(call, arg, ...)                                                                        \
    call(arg, 41) FASTCG_FOR_EACH_ITER_42_(FASTCG_FOR_EACH_ITER_41, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_43(call, arg, ...)                                                                        \
    call(arg, 42) FASTCG_FOR_EACH_ITER_43_(FASTCG_FOR_EACH_ITER_42, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_44(call, arg, ...)                                                                        \
    call(arg, 43) FASTCG_FOR_EACH_ITER_44_(FASTCG_FOR_EACH_ITER_43, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_45(call, arg, ...)                                                                        \
    call(arg, 44) FASTCG_FOR_EACH_ITER_45_(FASTCG_FOR_EACH_ITER_44, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_46(call, arg, ...)                                                                        \
    call(arg, 45) FASTCG_FOR_EACH_ITER_46_(FASTCG_FOR_EACH_ITER_45, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_47(call, arg, ...)                                                                        \
    call(arg, 46) FASTCG_FOR_EACH_ITER_47_(FASTCG_FOR_EACH_ITER_46, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_48(call, arg, ...)                                                                        \
    call(arg, 47) FASTCG_FOR_EACH_ITER_48_(FASTCG_FOR_EACH_ITER_47, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_49(call, arg, ...)                                                                        \
    call(arg, 48) FASTCG_FOR_EACH_ITER_49_(FASTCG_FOR_EACH_ITER_48, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_50(call, arg, ...)                                                                        \
    call(arg, 49) FASTCG_FOR_EACH_ITER_50_(FASTCG_FOR_EACH_ITER_49, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_51(call, arg, ...)                                                                        \
    call(arg, 50) FASTCG_FOR_EACH_ITER_51_(FASTCG_FOR_EACH_ITER_50, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_52(call, arg, ...)                                                                        \
    call(arg, 51) FASTCG_FOR_EACH_ITER_52_(FASTCG_FOR_EACH_ITER_51, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_53(call, arg, ...)                                                                        \
    call(arg, 52) FASTCG_FOR_EACH_ITER_53_(FASTCG_FOR_EACH_ITER_52, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_54(call, arg, ...)                                                                        \
    call(arg, 53) FASTCG_FOR_EACH_ITER_54_(FASTCG_FOR_EACH_ITER_53, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_55(call, arg, ...)                                                                        \
    call(arg, 54) FASTCG_FOR_EACH_ITER_55_(FASTCG_FOR_EACH_ITER_54, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_56(call, arg, ...)                                                                        \
    call(arg, 55) FASTCG_FOR_EACH_ITER_56_(FASTCG_FOR_EACH_ITER_55, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_57(call, arg, ...)                                                                        \
    call(arg, 56) FASTCG_FOR_EACH_ITER_57_(FASTCG_FOR_EACH_ITER_56, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_58(call, arg, ...)                                                                        \
    call(arg, 57) FASTCG_FOR_EACH_ITER_58_(FASTCG_FOR_EACH_ITER_57, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_59(call, arg, ...)                                                                        \
    call(arg, 58) FASTCG_FOR_EACH_ITER_59_(FASTCG_FOR_EACH_ITER_58, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_60(call, arg, ...)                                                                        \
    call(arg, 59) FASTCG_FOR_EACH_ITER_60_(FASTCG_FOR_EACH_ITER_59, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_61(call, arg, ...)                                                                        \
    call(arg, 60) FASTCG_FOR_EACH_ITER_61_(FASTCG_FOR_EACH_ITER_60, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_62(call, arg, ...)                                                                        \
    call(arg, 61) FASTCG_FOR_EACH_ITER_62_(FASTCG_FOR_EACH_ITER_61, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_63(call, arg, ...)                                                                        \
    call(arg, 62) FASTCG_FOR_EACH_ITER_63_(FASTCG_FOR_EACH_ITER_62, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_64(call, arg, ...)                                                                        \
    call(arg, 63) FASTCG_FOR_EACH_ITER_64_(FASTCG_FOR_EACH_ITER_63, (call, ##__VA_ARGS__))
#define FASTCG_FOR_EACH_ITER_65(...) error : FASTCG_FOR_EACH only supports up to 64 arguments
#define FASTCG_FOR_EACH(call, ...)                                                                                     \
    FASTCG_FOR_EACH_ITER_(                                                                                             \
        FASTCG_GET_LAST_ARG(                                                                                           \
            _0, ##__VA_ARGS__, FASTCG_FOR_EACH_ITER_64, FASTCG_FOR_EACH_ITER_65, FASTCG_FOR_EACH_ITER_63,              \
            FASTCG_FOR_EACH_ITER_62, FASTCG_FOR_EACH_ITER_61, FASTCG_FOR_EACH_ITER_60, FASTCG_FOR_EACH_ITER_59,        \
            FASTCG_FOR_EACH_ITER_58, FASTCG_FOR_EACH_ITER_57, FASTCG_FOR_EACH_ITER_56, FASTCG_FOR_EACH_ITER_55,        \
            FASTCG_FOR_EACH_ITER_54, FASTCG_FOR_EACH_ITER_53, FASTCG_FOR_EACH_ITER_52, FASTCG_FOR_EACH_ITER_51,        \
            FASTCG_FOR_EACH_ITER_50, FASTCG_FOR_EACH_ITER_49, FASTCG_FOR_EACH_ITER_48, FASTCG_FOR_EACH_ITER_47,        \
            FASTCG_FOR_EACH_ITER_46, FASTCG_FOR_EACH_ITER_45, FASTCG_FOR_EACH_ITER_44, FASTCG_FOR_EACH_ITER_43,        \
            FASTCG_FOR_EACH_ITER_42, FASTCG_FOR_EACH_ITER_41, FASTCG_FOR_EACH_ITER_40, FASTCG_FOR_EACH_ITER_39,        \
            FASTCG_FOR_EACH_ITER_38, FASTCG_FOR_EACH_ITER_37, FASTCG_FOR_EACH_ITER_36, FASTCG_FOR_EACH_ITER_35,        \
            FASTCG_FOR_EACH_ITER_34, FASTCG_FOR_EACH_ITER_33, FASTCG_FOR_EACH_ITER_32, FASTCG_FOR_EACH_ITER_31,        \
            FASTCG_FOR_EACH_ITER_30, FASTCG_FOR_EACH_ITER_29, FASTCG_FOR_EACH_ITER_28, FASTCG_FOR_EACH_ITER_27,        \
            FASTCG_FOR_EACH_ITER_26, FASTCG_FOR_EACH_ITER_25, FASTCG_FOR_EACH_ITER_24, FASTCG_FOR_EACH_ITER_23,        \
            FASTCG_FOR_EACH_ITER_22, FASTCG_FOR_EACH_ITER_21, FASTCG_FOR_EACH_ITER_20, FASTCG_FOR_EACH_ITER_19,        \
            FASTCG_FOR_EACH_ITER_18, FASTCG_FOR_EACH_ITER_17, FASTCG_FOR_EACH_ITER_16, FASTCG_FOR_EACH_ITER_15,        \
            FASTCG_FOR_EACH_ITER_14, FASTCG_FOR_EACH_ITER_13, FASTCG_FOR_EACH_ITER_12, FASTCG_FOR_EACH_ITER_11,        \
            FASTCG_FOR_EACH_ITER_10, FASTCG_FOR_EACH_ITER_09, FASTCG_FOR_EACH_ITER_08, FASTCG_FOR_EACH_ITER_07,        \
            FASTCG_FOR_EACH_ITER_06, FASTCG_FOR_EACH_ITER_05, FASTCG_FOR_EACH_ITER_04, FASTCG_FOR_EACH_ITER_03,        \
            FASTCG_FOR_EACH_ITER_02, FASTCG_FOR_EACH_ITER_01, FASTCG_FOR_EACH_ITER_00),                                \
        (call, ##__VA_ARGS__))

#define FASTCG_ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))
#define FASTCG_UNUSED(x) (void)(x)

#if _DEBUG
#if defined _MSC_VER
#define FASTCG_BREAK_TO_DEBUGGER() __debugbreak()
#elif defined __GNUC__ || defined __clang__
#define FASTCG_BREAK_TO_DEBUGGER() __builtin_trap()
#else
#error "FASTCG_BREAK_TO_DEBUGGER() macro is not implemented on the current compiler"
#endif
#else
#define FASTCG_BREAK_TO_DEBUGGER()
#endif

#if defined _MSC_VER
#define FASTCG_PACKED_PREFIX _Pragma("pack(push, 1)");
#define FASTCG_PACKED_SUFFIX                                                                                           \
    ;                                                                                                                  \
    _Pragma("pack(pop)")
#elif defined __GNUC__
#define FASTCG_PACKED_PREFIX
#define FASTCG_PACKED_SUFFIX __attribute__((packed))
#endif

#if defined _MSC_VER
#define FASTCG_COMPILER_WARN_PUSH _Pragma("warning(push)")
#define FASTCG_COMPILER_WARN_IGNORE_CRT_SECURITY_COMPLAINTS _Pragma("warning(disable:4996)")
#define FASTCG_COMPILER_WARN_IGNORE_DEPRECATED_DECLARATIONS
#define FASTCG_COMPILER_WARN_IGNORE_FORMAT_TRUNCATION
#define FASTCG_COMPILER_WARN_POP _Pragma("warning(pop)")
#elif defined __clang__
#define FASTCG_COMPILER_WARN_PUSH _Pragma("clang diagnostic push")
#define FASTCG_COMPILER_WARN_IGNORE_CRT_SECURITY_COMPLAINTS
#define FASTCG_COMPILER_WARN_IGNORE_DEPRECATED_DECLARATIONS                                                            \
    _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#define FASTCG_COMPILER_WARN_IGNORE_FORMAT_TRUNCATION
#define FASTCG_COMPILER_WARN_POP _Pragma("clang diagnostic pop")
#elif defined __GNUC__
#define FASTCG_COMPILER_WARN_PUSH _Pragma("GCC diagnostic push")
#define FASTCG_COMPILER_WARN_IGNORE_CRT_SECURITY_COMPLAINTS
#define FASTCG_COMPILER_WARN_IGNORE_DEPRECATED_DECLARATIONS                                                            \
    _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define FASTCG_COMPILER_WARN_IGNORE_FORMAT_TRUNCATION _Pragma("GCC diagnostic ignored \"-Wformat-truncation\"")
#define FASTCG_COMPILER_WARN_POP _Pragma("GCC diagnostic pop")
#else
#error "FASTCG_WARN_* macros are not implemented on the current compiler"
#endif

#endif