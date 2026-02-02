#include <cstddef>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

#include "helpers/string.hpp"

namespace helpers::string
{

std::vector<std::string> split(const std::string& str, char character)
{
    std::stringstream strStream = std::stringstream(str);
    std::vector<std::string> result;
    std::string currentStr;

    while (std::getline(strStream, currentStr, character).fail() == false)
    {
        result.push_back(currentStr);
    }

    return result;
}

std::string fold(const std::vector<std::string>& strs, char delimiter)
{
    std::stringstream strStream;
    std::size_t i;

    for (i = 0; i < strs.size(); ++i)
    {
        strStream << strs[i];

        if (i < (strs.size() - 1))
        {
            strStream << delimiter;
        }
    }

    return strStream.str();
}

int countOccurrencesOfCharacter(const std::string &str, char character)
{
    int result = 0;
    const char* strAsCStr = str.c_str();
    std::size_t strSize = str.size();
    std::size_t i;

    for (i = 0; i < strSize; ++i)
    {
        if (strAsCStr[i] == character)
        {
            result++;
        }
    }

    return result;
}

bool beginsWith(const std::string& str, const std::string& begin)
{
    std::size_t beginSize = begin.size();
    std::string subStr = str.substr(0, beginSize);
    bool result;

    if (subStr == begin)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

std::string merge(const std::string& strA, const std::string& strB, std::size_t numberOfOccurrencesOfCharacterInA, char character)
{
    std::stringstream result;
    std::vector<std::string> splittedStrA = split(strA, character);
    std::vector<std::string> splittedStrB = split(strB, character);
    std::size_t i;

    if (numberOfOccurrencesOfCharacterInA > splittedStrA.size())
    {
        result << strA;
    }
    else
    {
        for (i = 0; i < numberOfOccurrencesOfCharacterInA; ++i)
        {
            result << splittedStrA[i] << character;
        }

        for (i = numberOfOccurrencesOfCharacterInA; i < splittedStrB.size(); ++i)
        {
            result << splittedStrB[i] << character;
        }
    }

    return result.str();
}

} // namespace helpers::string