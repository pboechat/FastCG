#include <FastCG/Core/Colors.h>

#include <type_traits>

namespace FastCG
{
    const glm::vec4 Colors::NONE(0, 0, 0, 0);
    const glm::vec4 Colors::WHITE(1, 1, 1, 1);
    const glm::vec4 Colors::SILVER(0.75f, 0.75f, 0.75f, 1.0f);
    const glm::vec4 Colors::GRAY(0.5f, 0.5f, 0.5f, 1.0f);
    const glm::vec4 Colors::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
    const glm::vec4 Colors::RED(1.0f, 0.0f, 0.0f, 1.0f);
    const glm::vec4 Colors::MAROON(0.5f, 0.0f, 0.0f, 1.0f);
    const glm::vec4 Colors::YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
    const glm::vec4 Colors::OLIVE(0.5f, 0.5f, 0.0f, 1.0f);
    const glm::vec4 Colors::LIME(0.0f, 1.0f, 0.0f, 1.0f);
    const glm::vec4 Colors::GREEN(0.0f, 0.5f, 0.0f, 1.0f);
    const glm::vec4 Colors::AQUA(0.0f, 1.0f, 1.0f, 1.0f);
    const glm::vec4 Colors::TEAL(0.0f, 0.5f, 0.5f, 1.0f);
    const glm::vec4 Colors::BLUE(0.0f, 0.0f, 1.0f, 1.0f);
    const glm::vec4 Colors::NAVY(0.0f, 0.0f, 0.5f, 1.0f);
    const glm::vec4 Colors::FUCHSIA(1.0f, 0.0f, 1.0f, 1.0f);
    const glm::vec4 Colors::PURPLE(0.5f, 0.0f, 0.5f, 1.0f);
    const glm::vec4 Colors::COMMON_LIGHT_COLORS[] = {WHITE, RED,  MAROON, YELLOW, OLIVE,   LIME,  GREEN,
                                                     AQUA,  TEAL, BLUE,   NAVY,   FUCHSIA, PURPLE};
    const size_t Colors::NUMBER_OF_COMMON_LIGHT_COLORS = std::extent<decltype(COMMON_LIGHT_COLORS)>::value;

}
