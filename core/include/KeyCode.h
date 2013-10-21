#ifndef KEYCODE_H_
#define KEYCODE_H_

#include <Exception.h>

#include <windows.h>

class KeyCode
{
public:
	static const int BACKSPACE = VK_BACK;
	static const int RETURN = VK_RETURN;
	static const int ESCAPE = VK_ESCAPE;
	static const int SPACE = VK_SPACE;
	static const int ASTERISK = VK_MULTIPLY;
	static const int PLUS = VK_ADD;
	static const int COMMA = 110;
	static const int MINUS = VK_SUBTRACT;
	static const int DOT = 194;
	static const int SLASH = 111;
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
	static const int SEMI_COLON = 191;
	static const int EQUALS = 187;
	/* remapped special keys (62 - 90) */
	static const int LEFT_ARROW = VK_LEFT;
	static const int UP_ARROW = VK_UP;
	static const int RIGHT_ARROW = VK_RIGHT;
	static const int DOWN_ARROW = VK_DOWN;
	static const int F1 = 112;
	static const int F2 = 113;
	static const int F3 = 114;
	static const int F4 = 115;
	static const int F5 = 116;
	static const int F6 = 117;
	static const int F7 = 118;
	static const int F8 = 119;
	static const int F9 = 120;
	static const int F10 = 121;
	static const int F11 = 122;
	static const int F12 = 123;
	static const int PAGE_UP = 33;
	static const int PAGE_DOWN = 34;
	static const int HOME = VK_HOME;
	static const int END = VK_END;
	static const int INSERT = VK_INSERT;
	static const int SHIFT = VK_SHIFT;
	static const int RIGHT_SHIFT = VK_RSHIFT;
	static const int CONTROL = VK_CONTROL;
	static const int RIGHT_CONTROL = VK_RCONTROL;
	static const int ALT = 16;
	static const int RIGHT_ALT = 17;
	/* remapped special keys */
	static const int OPEN_SQUARE_BRACKET = 221;
	static const int BACKSLASH = 226;
	static const int CLOSE_SQUARE_BRACKET = 220;
	static const int LETTER_A = 65;
	static const int LETTER_B = 66;
	static const int LETTER_C = 67;
	static const int LETTER_D = 68;
	static const int LETTER_E = 69;
	static const int LETTER_F = 70;
	static const int LETTER_G = 71;
	static const int LETTER_H = 72;
	static const int LETTER_I = 73;
	static const int LETTER_J = 74;
	static const int LETTER_K = 75;
	static const int LETTER_L = 76;
	static const int LETTER_M = 77;
	static const int LETTER_N = 78;
	static const int LETTER_O = 79;
	static const int LETTER_P = 80;
	static const int LETTER_Q = 81;
	static const int LETTER_R = 82;
	static const int LETTER_S = 83;
	static const int LETTER_T = 84;
	static const int LETTER_U = 85;
	static const int LETTER_V = 86;
	static const int LETTER_W = 87;
	static const int LETTER_X = 88;
	static const int LETTER_Y = 89;
	static const int LETTER_Z = 90;
	static const int TILDE = 222;
	static const int DEL = VK_DELETE;

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