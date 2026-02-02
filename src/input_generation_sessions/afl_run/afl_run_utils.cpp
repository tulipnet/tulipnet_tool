#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "input_generation_sessions/afl_run/afl_run_utils.hpp"
#include "helpers/either.hpp"
#include "helpers/string.hpp"

namespace input_generation_sessions::afl_run::utils
{

std::map<std::string, std::string> getParametersFromAFLQueueFilename(std::string aflQueueFilename)
{
    std::map<std::string, std::string> result = std::map<std::string, std::string>();
    std::vector<std::string> splittedFilename = helpers::string::split(aflQueueFilename, ',');
    std::vector<std::string> currentSplittedParameter;

    for (const std::string& parameterToValue : splittedFilename)
    {
        currentSplittedParameter = helpers::string::split(parameterToValue, ':');

        if (currentSplittedParameter.size() == 2)
        {
            result.insert(std::pair<std::string, std::string>(currentSplittedParameter.at(0), currentSplittedParameter.at(1)));
        }
        else if (currentSplittedParameter.size() == 1)
        {
            result.insert(std::pair<std::string, std::string>("last", currentSplittedParameter.at(0)));
        }
    }

    return result;
}

std::string getAFLQueueFilenameParameterKey(AFLQueueFilenameParameter aflQueueFilenameParameter)
{
    std::string result;

    switch (aflQueueFilenameParameter)
    {
        case AFLQueueFilenameParameter::ID:
            result = "id";
            break;

        case AFLQueueFilenameParameter::WILDCARD:
            result = "w";
            break;

        case AFLQueueFilenameParameter::SYNC:
            result = "sync";
            break;

        case AFLQueueFilenameParameter::SRC_ID:
            result = "src";
            break;

        case AFLQueueFilenameParameter::TIME:
            result = "time";
            break;

        case AFLQueueFilenameParameter::EXECS:
            result = "execs";
            break;

        case AFLQueueFilenameParameter::OP:
            result = "op";
            break;

        case AFLQueueFilenameParameter::LAST:
            result = "last";
            break;
    }

    return result;
}

std::map<std::string, helpers::Either<std::string, std::vector<int>>> getParametersFromWildcardFilename(std::string wildcardFilename)
{
    using mappedType = helpers::Either<std::string, std::vector<int>>;

    std::map<std::string, mappedType> result = std::map<std::string, mappedType>();
    std::vector<std::string> splittedFilename = helpers::string::split(wildcardFilename, ';');
    std::vector<std::string> currentSplittedParameter;
    std::vector<std::string> splittedContent;
    std::vector<int> splittedContentInts;

    for (const std::string& parameterToValue : splittedFilename)
    {
        currentSplittedParameter = helpers::string::split(parameterToValue, '=');

        if (currentSplittedParameter.at(0) != "content")
        {
            result.insert(std::pair<std::string, std::string>(currentSplittedParameter.at(0), mappedType(currentSplittedParameter.at(1))));
        }
        else
        {
            splittedContent = helpers::string::split(currentSplittedParameter.at(1), '-');
            splittedContentInts = std::vector<int>(splittedContent.size());

            std::transform(splittedContent.begin(), splittedContent.end(), splittedContentInts.begin(),
                [](const std::string& contentStr)
                {
                    return std::stoi(contentStr);
                }
            );

            result.insert(std::pair<std::string, std::vector<int>>(currentSplittedParameter.at(0), mappedType(splittedContentInts)));
        }
    }

    return result;
}

std::string getAWildcardFilenameParameterKey(WildcardFilenameParameter wildcardFilenameParameter)
{
    std::string result;

    switch (wildcardFilenameParameter)
    {
        case WildcardFilenameParameter::WILDCARD_ID:
            result = "wid";
            break;

        case WildcardFilenameParameter::DEPTH:
            result = "depth";
            break;

        case WildcardFilenameParameter::CONTENT:
            result = "content";
            break;
    }

    return result;
}

} // namespace input_generation_sessions::afl_run::utils