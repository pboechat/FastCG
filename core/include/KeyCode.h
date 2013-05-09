#ifndef KEYCODE_H_
#define KEYCODE_H_

#include <Exception.h>

class KeyCode
{
public:
	static const int BACKSPACE = 8;
	static const int RETURN = 13;
	static const int ESCAPE = 27;
	static const int SPACE = 32;
	static const int ASTERISK = 42;
	static const int PLUS = 43;
	static const int COMMA = 44;
	static const int MINUS = 45;
	static const int DOT = 46;
	static const int SLASH = 47;
	static const int NUMBER_0 = 48;
	static const int NUMBER_1 = 49;
	static const int NUMBER_2 = 50;
	static const int NUMBER_3 = 51;
	static const int NUMBER_4 = 52;
	static const int NUMBER_5 = 53;
	static const int NUMBER_6 = 54;
	static const int NUMBER_7 = 55;
	static const int NUMBER_8 = 56;
	static const int NUMBER_9 = 57;
	static const int COLON = 58;
	static const int SEMI_COLON = 59;
	static const int EQUALS = 61;
	/* remapped special keys (62 - 90) */
	static const int LEFT_ARROW = 62;
	static const int UP_ARROW = 63;
	static const int RIGHT_ARROW = 64;
	static const int DOWN_ARROW = 65;
	static const int F1 = 66;
	static const int F2 = 67;
	static const int F3 = 68;
	static const int F4 = 69;
	static const int F5 = 70;
	static const int F6 = 71;
	static const int F7 = 72;
	static const int F8 = 73;
	static const int F9 = 74;
	static const int F10 = 75;
	static const int F11 = 76;
	static const int F12 = 77;
	static const int PAGE_UP = 78;
	static const int PAGE_DOWN = 79;
	static const int HOME = 80;
	static const int END = 81;
	static const int INSERT = 82;
	static const int SHIFT = 83;
	static const int RIGHT_SHIFT = 84;
	static const int CONTROL = 85;
	static const int RIGHT_CONTROL = 86;
	static const int ALT = 87;
	static const int RIGHT_ALT = 88;
	/* remapped special keys */
	static const int OPEN_SQUARE_BRACKET = 91;
	static const int BACKSLASH = 92;
	static const int CLOSE_SQUARE_BRACKET = 93;
	static const int LETTER_A = 97;
	static const int LETTER_B = 98;
	static const int LETTER_C = 99;
	static const int LETTER_D = 100;
	static const int LETTER_E = 101;
	static const int LETTER_F = 102;
	static const int LETTER_G = 103;
	static const int LETTER_H = 104;
	static const int LETTER_I = 105;
	static const int LETTER_J = 106;
	static const int LETTER_K = 107;
	static const int LETTER_L = 108;
	static const int LETTER_M = 109;
	static const int LETTER_N = 110;
	static const int LETTER_O = 111;
	static const int LETTER_P = 112;
	static const int LETTER_Q = 113;
	static const int LETTER_R = 114;
	static const int LETTER_S = 115;
	static const int LETTER_T = 116;
	static const int LETTER_U = 117;
	static const int LETTER_V = 118;
	static const int LETTER_W = 119;
	static const int LETTER_X = 120;
	static const int LETTER_Y = 121;
	static const int LETTER_Z = 122;
	static const int TILDE = 126;
	static const int DEL = 127;

	static int ToRegularKeyCode(int specialKeyCode)
	{
		if (specialKeyCode == SPECIAL_LEFT_ARROW)
		{
			return LEFT_ARROW;
		}
		else if (specialKeyCode == SPECIAL_UP_ARROW)
		{
			return UP_ARROW;
		}
		else if (specialKeyCode == SPECIAL_RIGHT_ARROW)
		{
			return RIGHT_ARROW;
		}
		else if (specialKeyCode == SPECIAL_DOWN_ARROW)
		{
			return DOWN_ARROW;
		}
		else if (specialKeyCode == SPECIAL_F1)
		{
			return F1;
		}
		else if (specialKeyCode == SPECIAL_F2)
		{
			return F2;
		}
		else if (specialKeyCode == SPECIAL_F3)
		{
			return F3;
		}
		else if (specialKeyCode == SPECIAL_F4)
		{
			return F4;
		}
		else if (specialKeyCode == SPECIAL_F5)
		{
			return F5;
		}
		else if (specialKeyCode == SPECIAL_F6)
		{
			return F6;
		}
		else if (specialKeyCode == SPECIAL_F7)
		{
			return F7;
		}
		else if (specialKeyCode == SPECIAL_F8)
		{
			return F8;
		}
		else if (specialKeyCode == SPECIAL_F9)
		{
			return F9;
		}
		else if (specialKeyCode == SPECIAL_F10)
		{
			return F10;
		}
		else if (specialKeyCode == SPECIAL_F11)
		{
			return F11;
		}
		else if (specialKeyCode == SPECIAL_F12)
		{
			return F12;
		}
		else if (specialKeyCode == SPECIAL_PAGE_UP)
		{
			return PAGE_UP;
		}
		else if (specialKeyCode == SPECIAL_PAGE_DOWN)
		{
			return PAGE_DOWN;
		}
		else if (specialKeyCode == SPECIAL_HOME)
		{
			return HOME;
		}
		else if (specialKeyCode == SPECIAL_END)
		{
			return END;
		}
		else if (specialKeyCode == SPECIAL_INSERT)
		{
			return INSERT;
		}
		else if (specialKeyCode == SPECIAL_SHIFT)
		{
			return SHIFT;
		}
		else if (specialKeyCode == SPECIAL_RIGHT_SHIFT)
		{
			return RIGHT_SHIFT;
		}
		else if (specialKeyCode == SPECIAL_CONTROL)
		{
			return CONTROL;
		}
		else if (specialKeyCode == SPECIAL_RIGHT_CONTROL)
		{
			return RIGHT_CONTROL;
		}
		else if (specialKeyCode == SPECIAL_ALT)
		{
			return ALT;
		}
		else if (specialKeyCode == SPECIAL_RIGHT_ALT)
		{
			return RIGHT_ALT;
		}
		else 
		{
			THROW_EXCEPTION(Exception, "Unmapped special key code: %d", specialKeyCode);
		}
	}
private:
	/* special keys */
	static const int SPECIAL_LEFT_ARROW = 100;
	static const int SPECIAL_UP_ARROW = 101;
	static const int SPECIAL_RIGHT_ARROW = 102;
	static const int SPECIAL_DOWN_ARROW = 103;
	static const int SPECIAL_F1 = 1;
	static const int SPECIAL_F2 = 2;
	static const int SPECIAL_F3 = 3;
	static const int SPECIAL_F4 = 4;
	static const int SPECIAL_F5 = 5;
	static const int SPECIAL_F6 = 6;
	static const int SPECIAL_F7 = 7;
	static const int SPECIAL_F8 = 8;
	static const int SPECIAL_F9 = 9;
	static const int SPECIAL_F10 = 10;
	static const int SPECIAL_F11 = 11;
	static const int SPECIAL_F12 = 12;
	static const int SPECIAL_PAGE_UP = 104;
	static const int SPECIAL_PAGE_DOWN = 105;
	static const int SPECIAL_HOME = 106;
	static const int SPECIAL_END = 107;
	static const int SPECIAL_INSERT = 108;
	static const int SPECIAL_SHIFT = 112;
	static const int SPECIAL_RIGHT_SHIFT = 113;
	static const int SPECIAL_CONTROL = 114;
	static const int SPECIAL_RIGHT_CONTROL = 115;
	static const int SPECIAL_ALT = 116;
	static const int SPECIAL_RIGHT_ALT = 117;

	KeyCode() {}
	~KeyCode() {}

};

#endif