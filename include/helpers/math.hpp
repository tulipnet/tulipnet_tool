#ifndef MATH_HPP
#define MATH_HPP

namespace helpers::math
{

/**
 * @brief Returns the minimal element between two elements by using the \f$ < \f$ operator of the elements type
 * 
 * @tparam T Type of the elements
 * @param a An element
 * @param b An element
 * @return T THe minimal element among the two provided
 */
template <typename T>
T min(const T& a, const T& b)
{
    T result;

    if (a < b)
    {
        result = a;
    }
    else
    {
        result = b;
    }

    return result;
}

} // namespace helpers::math

#endif