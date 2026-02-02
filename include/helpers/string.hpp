#ifndef STRING_HPP
#define STRING_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace helpers::string
{

/**
 * @brief Splits a string in a vector of string on a character
 * 
 * @param str The string to split
 * @param character The character on which to split the string
 * @return std::vector<std::string> The result
 */
std::vector<std::string> split(const std::string& str, char character);

/**
 * @brief Folds a vector of string by joining them with a provided delimiter. Does not add an extract delimiter at the end.
 * 
 * @param strs The vector of strings to fold
 * @param delimiter The delimiter to insert between each string
 * @return std::string The result
 */
std::string fold(const std::vector<std::string>& strs, char delimiter);

/**
 * @brief Counts the number of occurrences of a specific character in a string
 * 
 * @param str The string on which to count
 * @param character The character to count
 * @return int The result
 */
int countOccurrencesOfCharacter(const std::string& str, char character);

/**
 * @brief Tests if a string begins with a specific beginning
 * 
 * @param str The string the test
 * @param begin The beginning string to test
 * @return true `str` begins by the `begin` parameter
 * @return false `str` does not begin by the `begin` parameter
 */
bool beginsWith(const std::string& str, const std::string& begin);

/**
 * @brief Merge two strings by cutting the first one `numberOfOccurrencesOfCharacterInA` times on the character `character` (See split()).
 * 
 *        Example (`numberOfOccurrencesOfCharacterInA = 2`, `character = '\n'`):
 *        USER toto    USER *    USER toto
 *        PASS titi +  PASS * => PASS titi
 *        RETR         LIST      LIST
 * 
 * @param strA The first string
 * @param strB The second string
 * @param numberOfOccurrencesOfCharacterInA The number of occurrences of the character that will remain in the first string
 * @param character The character to check the number of occurrences
 * @return std::string The resulting string
 */
std::string merge(const std::string& strA, const std::string& strB, std::size_t numberOfOccurrencesOfCharacterInA, char character);

} // namespace helpers::string

#endif
