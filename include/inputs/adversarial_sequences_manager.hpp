#ifndef ADVERSARIAL_SEQUENCES_MANAGER_HPP
#define ADVERSARIAL_SEQUENCES_MANAGER_HPP

#include <filesystem>
#include <vector>

namespace inputs
{

class AdversarialSequencesManager;

} // namespace inputs

namespace inputs
{

class AdversarialSequencesManager
{
    private:
        static bool isAWildcardedAdversarialSequence(std::string adversarialSequence);

        std::string adversarialSequencesPath;

    public:
        AdversarialSequencesManager(std::string _adversarialSequencesPath);

        std::vector<std::filesystem::path> getWildcardedAdversarialSequences() const;
        std::vector<std::filesystem::path> getStaticAdversarialSequences() const;
};

} // namespace inputs

#endif