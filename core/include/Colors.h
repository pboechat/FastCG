#ifndef COLORS_H_
#define COLORS_H_

#include <glm/glm.hpp>

class Colors
{
public:
	static const glm::vec4 WHITE;
	static const glm::vec4 SILVER;
	static const glm::vec4 GRAY;
	static const glm::vec4 BLACK;
	static const glm::vec4 RED;
	static const glm::vec4 MAROON;
	static const glm::vec4 YELLOW;
	static const glm::vec4 OLIVE;
	static const glm::vec4 LIME;
	static const glm::vec4 GREEN;
	static const glm::vec4 AQUA;
	static const glm::vec4 TEAL;
	static const glm::vec4 BLUE;
	static const glm::vec4 NAVY;
	static const glm::vec4 FUCHSIA;
	static const glm::vec4 PURPLE;
	static const unsigned int NUMBER_OF_COMMON_LIGHT_COLORS;
	static const glm::vec4 COMMON_LIGHT_COLORS[];

private:
	Colors()
	{
	}

	~Colors()
	{
	}

};

#endif