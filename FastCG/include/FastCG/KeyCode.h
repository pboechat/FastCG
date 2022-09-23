#ifndef FASTCG_KEYCODE_H_
#define FASTCG_KEYCODE_H_

#include <FastCG/Exception.h>

#include <Windows.h>
#include <WinUser.h>

namespace FastCG
{
	class KeyCode
	{
	public:
#ifdef _WIN32
		static const int BACKSPACE = VK_BACK;
		static const int RETURN = VK_RETURN;
		static const int ESCAPE = VK_ESCAPE;
		static const int SPACE = VK_SPACE;
		static const int ASTERISK = VK_MULTIPLY;
		static const int PLUS = VK_ADD;
		static const int COMMA = VK_OEM_COMMA;
		static const int MINUS = VK_SUBTRACT;
		static const int DOT = VK_OEM_PERIOD;
		static const int SLASH = VK_DIVIDE;
		static const int NUMBER_0 = VK_NUMPAD0;
		static const int NUMBER_1 = VK_NUMPAD1;
		static const int NUMBER_2 = VK_NUMPAD2;
		static const int NUMBER_3 = VK_NUMPAD3;
		static const int NUMBER_4 = VK_NUMPAD4;
		static const int NUMBER_5 = VK_NUMPAD5;
		static const int NUMBER_6 = VK_NUMPAD6;
		static const int NUMBER_7 = VK_NUMPAD7;
		static const int NUMBER_8 = VK_NUMPAD8;
		static const int NUMBER_9 = VK_NUMPAD9;
		static const int COLON = 58;
		static const int SEMI_COLON = 59;
		static const int EQUALS = 61;
		static const int LEFT_ARROW = VK_LEFT;
		static const int UP_ARROW = VK_UP;
		static const int RIGHT_ARROW = VK_RIGHT;
		static const int DOWN_ARROW = VK_DOWN;
		static const int F1 = VK_F1;
		static const int F2 = VK_F2;
		static const int F3 = VK_F3;
		static const int F4 = VK_F4;
		static const int F5 = VK_F5;
		static const int F6 = VK_F6;
		static const int F7 = VK_F7;
		static const int F8 = VK_F8;
		static const int F9 = VK_F9;
		static const int F10 = VK_F10;
		static const int F11 = VK_F11;
		static const int F12 = VK_F12;
		static const int PAGE_UP = VK_PRIOR;
		static const int PAGE_DOWN = VK_NEXT;
		static const int END = VK_END;
		static const int HOME = VK_HOME;
		static const int INSERT = VK_INSERT;
		static const int SHIFT = VK_SHIFT;
		static const int RIGHT_SHIFT = 84;
		static const int CONTROL = VK_CONTROL;
		static const int RIGHT_CONTROL = 86;
		static const int ALT = 87;
		static const int RIGHT_ALT = 88;
		static const int OPEN_SQUARE_BRACKET = 91;
		static const int BACKSLASH = 92;
		static const int CLOSE_SQUARE_BRACKET = 93;
		static const int LETTER_A = 'A';
		static const int LETTER_B = 'B';
		static const int LETTER_C = 'C';
		static const int LETTER_D = 'D';
		static const int LETTER_E = 'E';
		static const int LETTER_F = 'F';
		static const int LETTER_G = 'G';
		static const int LETTER_H = 'H';
		static const int LETTER_I = 'I';
		static const int LETTER_J = 'J';
		static const int LETTER_K = 'K';
		static const int LETTER_L = 'L';
		static const int LETTER_M = 'M';
		static const int LETTER_N = 'N';
		static const int LETTER_O = 'O';
		static const int LETTER_P = 'P';
		static const int LETTER_Q = 'Q';
		static const int LETTER_R = 'R';
		static const int LETTER_S = 'S';
		static const int LETTER_T = 'T';
		static const int LETTER_U = 'U';
		static const int LETTER_V = 'V';
		static const int LETTER_W = 'W';
		static const int LETTER_X = 'X';
		static const int LETTER_Y = 'Y';
		static const int LETTER_Z = 'Z';
		static const int TILDE = 126;
		static const int DEL = VK_DELETE;
#else
#error "FastCG::KeyCode constants not implemented on the current platform"
#endif

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
				THROW_EXCEPTION(Exception, "Unhandled special key code: %d", specialKeyCode);
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

		KeyCode() = delete;
		~KeyCode() = delete;

	};

}

#endif