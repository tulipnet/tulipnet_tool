#include <filesystem>
#include <vector>

#include "inputs/adversarial_sequences_manager.hpp"
#include "helpers/filesystem.hpp"
#include "helpers/string.hpp"

namespace inputs
{

bool AdversarialSequencesManager::isAWildcardedAdversarialSequence(std::string adversarialSequence)
{
    bool result;

    if (helpers::string::countOccurrencesOfCharacter(adversarialSequence, '*') == 0)
    {
        result = false;
    }
    else
    {
        result = true;
    }

    return result;
}

AdversarialSequencesManager::AdversarialSequencesManager(std::string _adversarialSequencesPath) :
    adversarialSequencesPath(_adversarialSequencesPath)
{
}

std::vector<std::filesystem::path> AdversarialSequencesManager::getWildcardedAdversarialSequences() const
{
    std::vector<std::filesystem::path> result;
    std::filesystem::path currentAdversarialSequencePath;
    std::string currentAdversarialSequenceContent;

    for (const std::filesystem::directory_entry& adversarialSequenceFile : std::filesystem::directory_iterator(this->adversarialSequencesPath))
    {
        currentAdversarialSequencePath = adversarialSequenceFile.path();
        currentAdversarialSequenceContent = helpers::filesystem::getFileContent(currentAdversarialSequencePath);

        if (isAWildcardedAdversarialSequence(currentAdversarialSequenceContent) == true)
        {
            result.push_back(currentAdversarialSequencePath);
        }
    }

    return result;
}

std::vector<std::filesystem::path> AdversarialSequencesManager::getStaticAdversarialSequences() const
{
    std::vector<std::filesystem::path> result;
    std::filesystem::path currentAdversarialSequencePath;
    std::string currentAdversarialSequenceContent;

    for (const std::filesystem::directory_entry& adversarialSequenceFile : std::filesystem::directory_iterator(this->adversarialSequencesPath))
    {
        currentAdversarialSequencePath = adversarialSequenceFile.path();
        currentAdversarialSequenceContent = helpers::filesystem::getFileContent(currentAdversarialSequencePath);

        if (isAWildcardedAdversarialSequence(currentAdversarialSequenceContent) == false)
        {
            result.push_back(currentAdversarialSequencePath);
        }
    }

    return result;
}

} // namespace inputs