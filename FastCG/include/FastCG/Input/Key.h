#ifndef FASTCG_KEYCODE_H
#define FASTCG_KEYCODE_H

#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace FastCG
{
    enum class Key : uint8_t
    {
        UNKNOWN = 0,
        BACKSPACE = '\b',
        FIRST_CHAR_KEY = BACKSPACE,
        RETURN = '\r',
        ESCAPE = 27,
        SPACE = ' ',
        EXCLAMATION_MARK = '!',
        DOUBLE_QUOTES = '"',
        HASH = '#',
        DOLLAR = '$',
        AMPERSAND = '&',
        PERCENT = '%',
        APOSTROPHE = '\'',
        OPENING_ROUND_BRACKET = '(',
        CLOSING_ROUND_BRACKET = ')',
        ASTERISK = '*',
        PLUS = '+',
        COMMA = ',',
        MINUS = '-',
        DOT = '.',
        SLASH = '/',
        NUMBER_0 = '0',
        NUMBER_1 = '1',
        NUMBER_2 = '2',
        NUMBER_3 = '3',
        NUMBER_4 = '4',
        NUMBER_5 = '5',
        NUMBER_6 = '6',
        NUMBER_7 = '7',
        NUMBER_8 = '8',
        NUMBER_9 = '9',
        COLON = ':',
        SEMI_COLON = ';',
        LESS_THAN = '<',
        EQUALS = '=',
        GREATER_THAN = '>',
        QUESTION_MARK = '?',
        AT = '@',
        OPENING_SQUARE_BRACKET = '[',
        BACKSLASH = '\\',
        CLOSING_SQUARE_BRACKET = ']',
        CIRCUMFLEX = '^',
        UNDERSCORE = '_',
        GRAVE = '`',
        LETTER_A = 'a',
        LETTER_B = 'b',
        LETTER_C = 'c',
        LETTER_D = 'd',
        LETTER_E = 'e',
        LETTER_F = 'f',
        LETTER_G = 'g',
        LETTER_H = 'h',
        LETTER_I = 'i',
        LETTER_J = 'j',
        LETTER_K = 'k',
        LETTER_L = 'l',
        LETTER_M = 'm',
        LETTER_N = 'n',
        LETTER_O = 'o',
        LETTER_P = 'p',
        LETTER_Q = 'q',
        LETTER_R = 'r',
        LETTER_S = 's',
        LETTER_T = 't',
        LETTER_U = 'u',
        LETTER_V = 'v',
        LETTER_W = 'w',
        LETTER_X = 'x',
        LETTER_Y = 'y',
        LETTER_Z = 'z',
        OPENING_CURLY_BRACKET = '{',
        VERTICAL_BAR = '|',
        CLOSING_CURLY_BRACKET = '}',
        TILDE = '~',
        DEL = 127,
        LAST_CHAR_KEY = DEL,
        LEFT_ARROW,
        FIRST_VIRTUAL_KEY = LEFT_ARROW,
        UP_ARROW,
        RIGHT_ARROW,
        DOWN_ARROW,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        PRINT_SCREEN,
        PAGE_UP,
        PAGE_DOWN,
        END,
        HOME,
        INSERT,
        NUMLOCK,
        KEYPAD_0,
        KEYPAD_1,
        KEYPAD_2,
        KEYPAD_3,
        KEYPAD_4,
        KEYPAD_5,
        KEYPAD_6,
        KEYPAD_7,
        KEYPAD_8,
        KEYPAD_9,
        KEYPAD_DECIMAL,
        KEYPAD_DIVIDE,
        KEYPAD_MULTIPLY,
        KEYPAD_SUBTRACT,
        KEYPAD_ADD,
        SHIFT,
        FIRST_MODIFIER_KEY = SHIFT,
        CONTROL,
        LAST_MODIFIER_KEY = CONTROL,
        LAST_VIRTUAL_KEY = CONTROL,
    };

    using KeyInt = std::underlying_type_t<Key>;

#ifdef max
#undef max
#endif

    constexpr auto KEY_COUNT = std::numeric_limits<KeyInt>::max();

    inline bool IsCharKey(Key key)
    {
        return (KeyInt)key >= (KeyInt)Key::FIRST_CHAR_KEY && (KeyInt)key <= (KeyInt)Key::LAST_CHAR_KEY;
    }

    inline bool IsVirtualKey(Key key)
    {
        return (KeyInt)key >= (KeyInt)Key::FIRST_VIRTUAL_KEY && (KeyInt)key <= (KeyInt)Key::LAST_VIRTUAL_KEY;
    }

#ifdef IsModifierKey
#undef IsModifierKey
#endif

    inline bool IsModifierKey(Key key)
    {
        return (KeyInt)key >= (KeyInt)Key::FIRST_MODIFIER_KEY && (KeyInt)key <= (KeyInt)Key::LAST_MODIFIER_KEY;
    }

}

#endif