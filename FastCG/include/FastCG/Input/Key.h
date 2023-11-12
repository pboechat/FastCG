#ifndef FASTCG_KEYCODE_H
#define FASTCG_KEYCODE_H

#include <type_traits>
#include <limits>
#include <cstdint>

namespace FastCG
{
	enum class Key : uint8_t
	{
		UNKNOWN = 0,
		BACKSPACE,
		RETURN,
		ESCAPE,
		SPACE,
		ASTERISK,
		PLUS,
		COMMA,
		MINUS,
		DOT,
		SLASH,
		NUMBER_0,
		NUMBER_1,
		NUMBER_2,
		NUMBER_3,
		NUMBER_4,
		NUMBER_5,
		NUMBER_6,
		NUMBER_7,
		NUMBER_8,
		NUMBER_9,
		COLON,
		SEMI_COLON,
		EQUALS,
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
		OPEN_SQUARE_BRACKET,
		BACKSLASH,
		CLOSE_SQUARE_BRACKET,
		LETTER_A,
		LETTER_B,
		LETTER_C,
		LETTER_D,
		LETTER_E,
		LETTER_F,
		LETTER_G,
		LETTER_H,
		LETTER_I,
		LETTER_J,
		LETTER_K,
		LETTER_L,
		LETTER_M,
		LETTER_N,
		LETTER_O,
		LETTER_P,
		LETTER_Q,
		LETTER_R,
		LETTER_S,
		LETTER_T,
		LETTER_U,
		LETTER_V,
		LETTER_W,
		LETTER_X,
		LETTER_Y,
		LETTER_Z,
		TILDE,
		DEL,
	};

	using KeyInt = std::underlying_type_t<Key>;

#ifdef max
#undef max
#endif

	constexpr auto KEY_COUNT = std::numeric_limits<KeyInt>::max();

}

#endif