#ifndef ADVERSARIAL_SEQUENCE_HPP
#define ADVERSARIAL_SEQUENCE_HPP

#include <string>

namespace inputs
{

class AdversarialSequence;

} // namespace inputs

#include "all_commands_manager.hpp"

namespace inputs
{

class AdversarialSequence
{
    private:
        const AdversarialSequence* previousAdversarialSequence;
        int lastCommandId;
        std::string rawResultsPath;
        const AllCommandsManager& allCommandsManager;
        bool isAStaticSequence;
        std::string finalResultsPath;

        void generateRawResultsPath();
        void determineIfItIsAStaticSequence();
        void generateWildcardFiles() const;
        void generateFinalResultsPath();

    public:
        AdversarialSequence(const AllCommandsManager& _allCommandsManager, std::string _firstCommand);
        AdversarialSequence(const AdversarialSequence* _previousAdversarialSequence, std::string _nextCommand);

        int getDepth() const;
        int getID() const;
        std::string getRawResultsPath() const;
        std::string getWildcardPath() const;
        bool getIfItIsAStaticSequence() const;
        std::string getFinalResultsPath() const;
};

} // namespace inputs

#endif