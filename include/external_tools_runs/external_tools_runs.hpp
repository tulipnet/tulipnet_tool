#include <string>

namespace external_tools_runs
{

/**
 * @brief Run the Apply Wildcard companion binary.
 * 
 * @param wildcardPath Path to the Wildcard file.
 * @param inputPath Path to the generated input by AFL++.
 * @param outputPath Output path to store the result.
 */
void runApplyWildcard(std::string wildcardPath, std::string inputPath, std::string outputPath);

/**
 * @brief Run the tool to concretize all wildcards of a Wildcard file, except the first.
 * 
 * @param wildcardPath Path to the Wildcard file.
 * @param concreteData Concrete data to put (Example : aaaa).
 * @param outputPath Output path to store the result.
 */
void runPutConcreteDataOn2ndAndNextWildcards(std::string wildcardPath, std::string concreteData, std::string outputPath);

/**
 * @brief Run the tool to concretize all wildcards of a Wildcard file.
 * 
 * @param wildcardPath Path to the Wildcard file.
 * @param concreteData Concrete data to put (Example : aaaa).
 * @param outputPath Output path to store the result.
 */
void runConcretizeWildcard(std::string wildcardPath, std::string concreteData, std::string outputPath);

} // namespace external_tools_runs