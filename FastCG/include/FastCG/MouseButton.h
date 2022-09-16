#ifndef FASTCG_MOUSE_BUTTON_H_
#define FASTCG_MOUSE_BUTTON_H_

#include <cstdint>

namespace FastCG
{
	enum class MouseButton : uint8_t
	{
		LEFT_BUTTON = 0,
		MIDDLE_BUTTON,
		RIGHT_BUTTON,

	};

	enum class MouseButtonState : uint8_t
	{
		PRESSED = 0,
		RELEASED,

	};

}

#endif