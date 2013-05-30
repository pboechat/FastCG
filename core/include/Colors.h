#ifndef COLORS_H_
#define COLORS_H_

#include <glm/glm.hpp>

class Colors
{
public:
	static const glm::vec4 WHITE;
	static const glm::vec4 BLACK;
	static const glm::vec4 RED;
	static const glm::vec4 GREEN;
	static const glm::vec4 BLUE;
	static const glm::vec4 YELLOW;
	static const glm::vec4 PURPLE;

private:
	Colors()
	{
	}

	~Colors()
	{
	}

};

#endif