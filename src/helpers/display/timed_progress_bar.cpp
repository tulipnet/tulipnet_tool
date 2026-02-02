#include <thread>
#include <unistd.h>

#include "helpers/display/timed_progress_bar.hpp"

namespace helpers::display
{

TimedProgressBar::TimedProgressBar(const std::string& _header, float _time) :
    ProgressBar(_header),
    totalTime(_time),
    currentTime(0)
{
}

TimedProgressBar::TimedProgressBar(Colors _headerColor, const std::string& _header, float _time) :
    ProgressBar(_headerColor, _header),
    totalTime(_time),
    currentTime(0)
{
}

void TimedProgressBar::showAndStartIncrementingBar()
{
    this->thread = std::thread([&]() {
        while (this->currentTime < this->totalTime)
        {
            this->increasePercentage((1 / this->totalTime) * 100);
            this->show();

            sleep(1);
            ++this->currentTime;
        }
    });
}

void TimedProgressBar::joinAssociatedThread()
{
    this->thread.join();
}

}