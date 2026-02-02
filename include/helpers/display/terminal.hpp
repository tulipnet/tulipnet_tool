#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <ostream>
#include <utility>

namespace helpers::display
{

/**
 * @brief Enum representing colors of the classical UNIX terminal, in normal and bold variants.
 * 
 */
enum class Colors : int
{
    NEUTRAL = 0x000,
    BLACK = 0x01e,
    RED = 0x01f,
    GREEN = 0x020,
    YELLOW = 0x021,
    BLUE = 0x022,
    MAGENTA = 0x023,
    CYAN = 0x024,
    WHITE = 0x025,
    NEUTRAL_BOLD = 0x100,
    BLACK_BOLD = 0x11e,
    RED_BOLD = 0x11f,
    GREEN_BOLD = 0x120,
    YELLOW_BOLD = 0x121,
    BLUE_BOLD = 0x122,
    MAGENTA_BOLD = 0x123,
    CYAN_BOLD = 0x124,
    WHITE_BOLD = 0x125
};

/**
 * @brief Get the width and the height of the terminal running the program.
 * 
 * @return std::pair<int, int> \f$ (a, b) \f$ such as \f$ a \f$ is the number of columns, and \f$ b \f$ the number of rows of the terminal in pixels.
 */
std::pair<int, int> getWidthAndHeightOfTheTerminal();

/**
 * @brief Straight-forward operator to easily use colors with `std::ostream` output streams (In particular, `cout`).
 * 
 * @param stream The stream on which apply the color.
 * @param color The color (Member of Colors) to apply.
 * @return std::ostream& The same stream as `stream` with the applied color.
 */
std::ostream& operator<<(std::ostream& stream, Colors color);

} // namespace helpers::display

#endif