#ifndef AFL_RUN_UTILS_HPP
#define AFL_RUN_UTILS_HPP

#include "helpers/either.hpp"
#include <map>
#include <string>
#include <vector>

namespace input_generation_sessions::afl_run::utils
{

enum class AFLQueueFilenameParameter
{
    ID,
    WILDCARD,
    SYNC,
    SRC_ID,
    TIME,
    EXECS,
    OP,
    LAST
};

enum class WildcardFilenameParameter
{
    WILDCARD_ID,
    DEPTH,
    CONTENT
};

// Example of input : id:000029,w:5.cmd,src:000002,time:874,execs:13376,op:havoc,rep:5
std::map<std::string, std::string> getParametersFromAFLQueueFilename(std::string aflQueueFilename);
std::string getAFLQueueFilenameParameterKey(AFLQueueFilenameParameter aflQueueFilenameParameter);
std::map<std::string, helpers::Either<std::string, std::vector<int>>> getParametersFromWildcardFilename(std::string wildcardFilename);
std::string getAWildcardFilenameParameterKey(WildcardFilenameParameter wildcardFilenameParameter);

} // namespace input_generation_sessions::afl_run::utils

#endif