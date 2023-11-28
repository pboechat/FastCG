#ifndef FASTCG_KEYCODE_H
#define FASTCG_KEYCODE_H

#include <type_traits>
#include <limits>
#include <cstdint>
#include <cassert>

namespace FastCG
{
	enum class Key : uint8_t
	{
		UNKNOWN = 0,
		BACKSPACE,
		DEL,
		RETURN,
		ESCAPE,
		LEFT_ARROW,
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
		PAGE_UP,
		PAGE_DOWN,
		END,
		HOME,
		INSERT,
		SHIFT,
		CONTROL,
		SPACE = 32,
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
		EQUALS = '=',
		BACKSLASH = '\\',
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
		FIRST_CHAR_KEY = SPACE,
		LAST_CHAR_KEY = LETTER_Z
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

}

#endif