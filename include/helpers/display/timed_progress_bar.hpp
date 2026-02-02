#ifndef TIMED_PROGRESS_BAR
#define TIMED_PROGRESS_BAR

#include <thread>

#include "helpers/display/progress_bar.hpp"

namespace helpers::display
{

/**
 * @brief A ProgressBar that evolves with the time (In seconds)
 * 
 */
class TimedProgressBar : public ProgressBar
{
    private:
        float totalTime;
        float currentTime;
        std::thread thread;

    public:
        /**
         * @brief Construct a new Timed Progress Bar object
         * 
         * @param _header Header of the new ProgressBar.
         * @param _time Total time in seconds to be completed.
         */
        TimedProgressBar(const std::string& _header, float _time);

        /**
         * @brief Construct a new Timed Progress Bar object
         * 
         * @param _headerColor A Colors enum member representing the color of the header of the newProgressBar.
         * @param _header Header of the new ProgressBar.
         * @param _time Total time in seconds to be completed.
         */
        TimedProgressBar(Colors _headerColor, const std::string& _header, float _time);

        /**
         * @brief Spawns an `std::thread` increasing the ProgressBar across the time.
         * 
         */
        void showAndStartIncrementingBar();

        /**
         * @brief Joins the associated `std::thread`.
         * 
         */
        void joinAssociatedThread();
};

}

#endif