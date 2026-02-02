#include <asm-generic/ioctls.h>
#include <ostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>

#include "helpers/display/terminal.hpp"

namespace helpers::display
{

std::pair<int, int> getWidthAndHeightOfTheTerminal()
{
    struct winsize terminalInfos;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminalInfos);

    return std::pair<int, int>(terminalInfos.ws_col, terminalInfos.ws_row);
}

std::ostream& operator<<(std::ostream& stream, Colors color)
{
    int colorAsInt = (int)color;
    int colorPrefix = colorAsInt >> 8;
    int colorSuffix = colorAsInt & 0x0ff;

    return stream << "\x1b[" << colorPrefix << ';' << colorSuffix << 'm';
}

} // namespace helpers::display