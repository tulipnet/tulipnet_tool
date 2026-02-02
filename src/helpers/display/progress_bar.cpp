#include <cstddef>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <utility>

#include "helpers/display/progress_bar.hpp"
#include "helpers/display/terminal.hpp"

namespace helpers::display
{

void ProgressBar::showBar() const
{
    std::pair<int, int> terminalWidthAndHeight = getWidthAndHeightOfTheTerminal();
    std::size_t headerSize = this->header.size() + 1; // + 1 for the ' ' at the end of the header
    std::size_t progressBarSize = terminalWidthAndHeight.first - headerSize;
    std::size_t numberOfCharactersInProgressBar = (this->currentPercentage / 100) * (progressBarSize - 2); // - 2 for '[' and ']'
    std::stringstream progressBarContent;
    std::size_t remainingWhitespacesToAdd;
    std::size_t i;

    progressBarContent << '[';

    if (numberOfCharactersInProgressBar > 0) // To prevent integer overflow...
    {
        for (i = 0; i < (numberOfCharactersInProgressBar - 1); ++i) // - 1 to add a '>' instead of an '='
        {
            progressBarContent << '=';
        }
    }

    progressBarContent << '>';

    if (numberOfCharactersInProgressBar == 0)
    {
        remainingWhitespacesToAdd = progressBarSize - 3; // - 2 for '[', ']' and '>' in the progress bar
    }
    else
    {
        remainingWhitespacesToAdd = (progressBarSize - numberOfCharactersInProgressBar - 2); // - 2 for '[' and ']' in the progress bar
    }

    for (i = 0; i < remainingWhitespacesToAdd; ++i)
    {
        progressBarContent << ' ';
    }

    progressBarContent << ']';

    std::cout << '\r' << this->headerColor << this->header << Colors::NEUTRAL << ' ' << progressBarContent.str();
    std::cout.flush();
}

void ProgressBar::showText() const
{
    std::cout << this->header << ' ' << this->currentPercentage << '%' << std::endl;
}

ProgressBar::ProgressBar(const std::string& _header) :
    header(_header),
    headerColor(Colors::NEUTRAL),
    currentPercentage(0)
{
}

ProgressBar::ProgressBar(Colors _headerColor, const std::string& _header) :
    header(_header),
    headerColor(_headerColor),
    currentPercentage(0)
{
}

void ProgressBar::show() const
{
    if (isatty(STDOUT_FILENO) == 1) // If STDOUT refers to a real terminal (Particularly, if stout is not piped to a file or tee)
    {
        this->showBar();
    }
    else
    {
        this->showText();
    }
}

void ProgressBar::increasePercentage(float increment)
{
    this->currentPercentage += increment;

    if (this->currentPercentage > 100)
    {
        this->currentPercentage = 100;
    }
    else if (this->currentPercentage < 0)
    {
        this->currentPercentage = 0;
    }
}

float ProgressBar::getCurrentPercentage() const
{
    return this->currentPercentage;
}

} // namespace helpers::display