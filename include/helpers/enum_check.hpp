#ifndef ENUM_CHECK_HPP
#define ENUM_CHECK_HPP

namespace helpers
{

/**
 * @brief Pseudo-class to check if a value is contained in an enumeration.
 * 
 * @tparam EnumType Type of the enumeration.
 * @tparam values Values to test.
 */
template <typename EnumType, EnumType... values>
class EnumCheck;

/**
 * @brief Base case of the pseudo-class to check if a value is contained in an enumeration.
 * 
 * @tparam EnumType Type of the enumeration.
 */
template <typename EnumType>
class EnumCheck<EnumType>
{
    public:
        /**
         * @brief Test if the passed value is valid for the enumeration of type EnumType.
         * 
         * @tparam IntType Type of the tested value.
         * @param value The tested value itself.
         * @return true The tested value is contained in the enumeration EnumType.
         * @return false The tested value is not contained in the enumeration EnumType.
         */
        template<typename IntType>
        static bool constexpr isValid([[maybe_unused]] IntType value)
        {
            return false;
        }
};

/**
 * @brief General case of the pseudo-class to check if a value is contained in an enumeration.
 * 
 * @tparam EnumType Type of the enumeration.
 * @tparam currentEnumerationValue Current enumeration value to test against.
 * @tparam nextValues Collector of next values to test against.
 */
template <typename EnumType, EnumType currentEnumerationValue, EnumType... nextValues>
class EnumCheck<EnumType, currentEnumerationValue, nextValues...> : private EnumCheck<EnumType, nextValues...>
{
    public:
        /**
         * @brief Test if the passed value is valid for the enumeration of type EnumType.
         * 
         * @tparam IntType Type of the tested value.
         * @param value The tested value itself.
         * @return true The tested value is contained in the enumeration EnumType.
         * @return false The tested value is not contained in the enumeration EnumType.
         */
        template<typename IntType>
        static bool constexpr isValid(IntType value)
        {
            return ((value == static_cast<IntType>(currentEnumerationValue)) || (EnumCheck<EnumType, nextValues...>::isValid(value) == true));
        }
};

} // namespace helpers

#endif