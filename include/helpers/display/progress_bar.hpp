#ifndef PROGRESS_BAR_HPP
#define PROGRESS_BAR_HPP

#include <string>

#include "helpers/display/terminal.hpp"

namespace helpers::display
{

/**
 * @brief Represent a responsive progress bar. The progress bar will have a header.
 * Example :
 * ```
 * [CBD stage 1 (final)] [==========>           ]
 * ```
 * The header corresponds to the part `[CBD stage 1 (final)]`.
 * The header can also be colorized (See constructors...).
 *
 * The completion of the progress bar is represented by a float \f$ p \in [0; 100] \f$.
 */
class ProgressBar
{
    private:
        const std::string& header;
        Colors headerColor;
        float currentPercentage;

        void showBar() const;
        void showText() const;

    public:
        /**
         * @brief Construct a new ProgressBar object with the specified header.
         * 
         * @param _header Header of the new ProgressBar.
         */
        ProgressBar(const std::string& _header);

        /**
         * @brief Construct a new Progress Bar object.
         * 
         * @param _headerColor A Colors enum member representing the color of the header of the newProgressBar.
         * @param _header Header of the new ProgressBar.
         */
        ProgressBar(Colors _headerColor, const std::string& _header);

        /**
         * @brief Show the ProgressBar on stdout.
         * 
         */
        void show() const;

        /**
         * @brief Increase the percentage of completion of the ProgressBar.
         * 
         * @param increment Increment of the ProgressBar completion.
         */
        void increasePercentage(float increment);

        /**
         * @brief Get the percentage of completion of the ProgressBar.
         * 
         * @return The percentage of completion of the ProgressBar.
         */
        float getCurrentPercentage() const;
};

} // namespace helpers::display

#endif