#ifndef FASTCG_KEYCODE_H
#define FASTCG_KEYCODE_H

#include <FastCG/FastCG.h>
#include <FastCG/Exception.h>

#if defined FASTCG_WINDOWS
#include <WinUser.h>
#elif defined FASTCG_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

namespace FastCG
{
	class KeyCodes
	{
	public:
#if defined FASTCG_WINDOWS
		static constexpr int BACKSPACE = VK_BACK;
		static constexpr int RETURN = VK_RETURN;
		static constexpr int ESCAPE = VK_ESCAPE;
		static constexpr int SPACE = VK_SPACE;
		static constexpr int ASTERISK = VK_MULTIPLY;
		static constexpr int PLUS = VK_ADD;
		static constexpr int COMMA = VK_OEM_COMMA;
		static constexpr int MINUS = VK_SUBTRACT;
		static constexpr int DOT = VK_OEM_PERIOD;
		static constexpr int SLASH = VK_DIVIDE;
		static constexpr int NUMBER_0 = VK_NUMPAD0;
		static constexpr int NUMBER_1 = VK_NUMPAD1;
		static constexpr int NUMBER_2 = VK_NUMPAD2;
		static constexpr int NUMBER_3 = VK_NUMPAD3;
		static constexpr int NUMBER_4 = VK_NUMPAD4;
		static constexpr int NUMBER_5 = VK_NUMPAD5;
		static constexpr int NUMBER_6 = VK_NUMPAD6;
		static constexpr int NUMBER_7 = VK_NUMPAD7;
		static constexpr int NUMBER_8 = VK_NUMPAD8;
		static constexpr int NUMBER_9 = VK_NUMPAD9;
		static constexpr int COLON = 58;
		static constexpr int SEMI_COLON = 59;
		static constexpr int EQUALS = 61;
		static constexpr int LEFT_ARROW = VK_LEFT;
		static constexpr int UP_ARROW = VK_UP;
		static constexpr int RIGHT_ARROW = VK_RIGHT;
		static constexpr int DOWN_ARROW = VK_DOWN;
		static constexpr int F1 = VK_F1;
		static constexpr int F2 = VK_F2;
		static constexpr int F3 = VK_F3;
		static constexpr int F4 = VK_F4;
		static constexpr int F5 = VK_F5;
		static constexpr int F6 = VK_F6;
		static constexpr int F7 = VK_F7;
		static constexpr int F8 = VK_F8;
		static constexpr int F9 = VK_F9;
		static constexpr int F10 = VK_F10;
		static constexpr int F11 = VK_F11;
		static constexpr int F12 = VK_F12;
		static constexpr int PAGE_UP = VK_PRIOR;
		static constexpr int PAGE_DOWN = VK_NEXT;
		static constexpr int END = VK_END;
		static constexpr int HOME = VK_HOME;
		static constexpr int INSERT = VK_INSERT;
		static constexpr int SHIFT = VK_SHIFT;
		static constexpr int RIGHT_SHIFT = 84;
		static constexpr int CONTROL = VK_CONTROL;
		static constexpr int RIGHT_CONTROL = 86;
		static constexpr int ALT = 87;
		static constexpr int RIGHT_ALT = 88;
		static constexpr int OPEN_SQUARE_BRACKET = 91;
		static constexpr int BACKSLASH = 92;
		static constexpr int CLOSE_SQUARE_BRACKET = 93;
		static constexpr int LETTER_A = 'A';
		static constexpr int LETTER_B = 'B';
		static constexpr int LETTER_C = 'C';
		static constexpr int LETTER_D = 'D';
		static constexpr int LETTER_E = 'E';
		static constexpr int LETTER_F = 'F';
		static constexpr int LETTER_G = 'G';
		static constexpr int LETTER_H = 'H';
		static constexpr int LETTER_I = 'I';
		static constexpr int LETTER_J = 'J';
		static constexpr int LETTER_K = 'K';
		static constexpr int LETTER_L = 'L';
		static constexpr int LETTER_M = 'M';
		static constexpr int LETTER_N = 'N';
		static constexpr int LETTER_O = 'O';
		static constexpr int LETTER_P = 'P';
		static constexpr int LETTER_Q = 'Q';
		static constexpr int LETTER_R = 'R';
		static constexpr int LETTER_S = 'S';
		static constexpr int LETTER_T = 'T';
		static constexpr int LETTER_U = 'U';
		static constexpr int LETTER_V = 'V';
		static constexpr int LETTER_W = 'W';
		static constexpr int LETTER_X = 'X';
		static constexpr int LETTER_Y = 'Y';
		static constexpr int LETTER_Z = 'Z';
		static constexpr int TILDE = 126;
		static constexpr int DEL = VK_DELETE;
#elif defined FASTCG_LINUX
		static constexpr int BACKSPACE = XK_BackSpace;
		static constexpr int RETURN = XK_Return;
		static constexpr int ESCAPE = XK_Escape;
		static constexpr int SPACE = XK_space;
		static constexpr int ASTERISK = XK_asterisk;
		static constexpr int PLUS = XK_plus;
		static constexpr int COMMA = XK_comma;
		static constexpr int MINUS = XK_minus;
		static constexpr int DOT = XK_period;
		static constexpr int SLASH = XK_slash;
		static constexpr int NUMBER_0 = XK_KP_0;
		static constexpr int NUMBER_1 = XK_KP_1;
		static constexpr int NUMBER_2 = XK_KP_2;
		static constexpr int NUMBER_3 = XK_KP_3;
		static constexpr int NUMBER_4 = XK_KP_4;
		static constexpr int NUMBER_5 = XK_KP_5;
		static constexpr int NUMBER_6 = XK_KP_6;
		static constexpr int NUMBER_7 = XK_KP_7;
		static constexpr int NUMBER_8 = XK_KP_8;
		static constexpr int NUMBER_9 = XK_KP_9;
		static constexpr int COLON = XK_colon;
		static constexpr int SEMI_COLON = XK_semicolon;
		static constexpr int EQUALS = XK_equal;
		static constexpr int LEFT_ARROW = XK_Left;
		static constexpr int UP_ARROW = XK_Up;
		static constexpr int RIGHT_ARROW = XK_Right;
		static constexpr int DOWN_ARROW = XK_Down;
		static constexpr int F1 = XK_F1;
		static constexpr int F2 = XK_F2;
		static constexpr int F3 = XK_F3;
		static constexpr int F4 = XK_F4;
		static constexpr int F5 = XK_F5;
		static constexpr int F6 = XK_F6;
		static constexpr int F7 = XK_F7;
		static constexpr int F8 = XK_F8;
		static constexpr int F9 = XK_F9;
		static constexpr int F10 = XK_F10;
		static constexpr int F11 = XK_F11;
		static constexpr int F12 = XK_F12;
		static constexpr int PAGE_UP = XK_Page_Up;
		static constexpr int PAGE_DOWN = XK_Page_Down;
		static constexpr int END = XK_End;
		static constexpr int HOME = XK_Home;
		static constexpr int INSERT = XK_Insert;
		static constexpr int SHIFT = XK_Shift_L;
		static constexpr int RIGHT_SHIFT = XK_Shift_R;
		static constexpr int CONTROL = XK_Control_L;
		static constexpr int RIGHT_CONTROL = XK_Control_R;
		static constexpr int ALT = XK_Alt_L;
		static constexpr int RIGHT_ALT = XK_Alt_R;
		static constexpr int OPEN_SQUARE_BRACKET = XK_bracketleft;
		static constexpr int BACKSLASH = XK_backslash;
		static constexpr int CLOSE_SQUARE_BRACKET = XK_bracketright;
		static constexpr int LETTER_A = XK_A;
		static constexpr int LETTER_B = XK_B;
		static constexpr int LETTER_C = XK_C;
		static constexpr int LETTER_D = XK_D;
		static constexpr int LETTER_E = XK_E;
		static constexpr int LETTER_F = XK_F;
		static constexpr int LETTER_G = XK_G;
		static constexpr int LETTER_H = XK_H;
		static constexpr int LETTER_I = XK_I;
		static constexpr int LETTER_J = XK_J;
		static constexpr int LETTER_K = XK_K;
		static constexpr int LETTER_L = XK_L;
		static constexpr int LETTER_M = XK_M;
		static constexpr int LETTER_N = XK_N;
		static constexpr int LETTER_O = XK_O;
		static constexpr int LETTER_P = XK_P;
		static constexpr int LETTER_Q = XK_Q;
		static constexpr int LETTER_R = XK_R;
		static constexpr int LETTER_S = XK_S;
		static constexpr int LETTER_T = XK_T;
		static constexpr int LETTER_U = XK_U;
		static constexpr int LETTER_V = XK_V;
		static constexpr int LETTER_W = XK_W;
		static constexpr int LETTER_X = XK_X;
		static constexpr int LETTER_Y = XK_Y;
		static constexpr int LETTER_Z = XK_Z;
		static constexpr int TILDE = XK_itilde;
		static constexpr int DEL = XK_Delete;
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
				FASTCG_THROW_EXCEPTION(Exception, "Unhandled special key code: %d", specialKeyCode);
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

		KeyCodes() = delete;
		~KeyCodes() = delete;
	};

}

#endif