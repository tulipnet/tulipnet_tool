#include <string>
#include <optional>

#include "external_tools_runs/external_tools_runs.hpp"
#include "helpers/runnables/external_binary_execution.hpp"
#include "kernel/constants.hpp"

namespace external_tools_runs
{

void runApplyWildcard(std::string wildcardPath, std::string inputPath, std::string outputPath)
{
    std::optional<helpers::runnables::ExternalBinaryExecution> reverseWildcardExecution;
    std::vector<std::string> reverseWildcardExecutionArgs;

    reverseWildcardExecutionArgs.push_back(wildcardPath);
    reverseWildcardExecutionArgs.push_back(inputPath);

    reverseWildcardExecution = helpers::runnables::ExternalBinaryExecution(APPLY_WILDCARD_EXECUTABLE_PATH, reverseWildcardExecutionArgs, outputPath, "/dev/null");

    reverseWildcardExecution->run();
    reverseWildcardExecution->wait();
}

void runPutConcreteDataOn2ndAndNextWildcards(std::string wildcardPath, std::string concreteData, std::string outputPath)
{
    std::optional<helpers::runnables::ExternalBinaryExecution> putConcreteDataOn2ndAndNextWildcardsExecution;
    std::vector<std::string> putConcreteDataOn2ndAndNextWildcardsExecutionArgs;

    putConcreteDataOn2ndAndNextWildcardsExecutionArgs.push_back(wildcardPath);
    putConcreteDataOn2ndAndNextWildcardsExecutionArgs.push_back(concreteData);

    putConcreteDataOn2ndAndNextWildcardsExecution = helpers::runnables::ExternalBinaryExecution(PUT_CONCRETE_DATA_ON_2ND_AND_NEXT_WILDCARDS_EXECUTABLE_PATH, putConcreteDataOn2ndAndNextWildcardsExecutionArgs, outputPath, "/dev/null");

    putConcreteDataOn2ndAndNextWildcardsExecution->run();
    putConcreteDataOn2ndAndNextWildcardsExecution->wait();
}

void runConcretizeWildcard(std::string wildcardPath, std::string concreteData, std::string outputPath)
{
    std::optional<helpers::runnables::ExternalBinaryExecution> concretizeWildcardExecution;
    std::vector<std::string> concretizeWildcardExecutionArgs;

    concretizeWildcardExecutionArgs.push_back(wildcardPath);
    concretizeWildcardExecutionArgs.push_back(concreteData);

    concretizeWildcardExecution = helpers::runnables::ExternalBinaryExecution(CONCRETIZE_WILDCARD_EXECUTABLE_PATH, concretizeWildcardExecutionArgs, outputPath, "/dev/null");

    concretizeWildcardExecution->run();
    concretizeWildcardExecution->wait();
}
} // namespace external_tools_runs