#ifndef GENERATED_INPUTS_MANAGER_HPP
#define GENERATED_INPUTS_MANAGER_HPP

#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace inputs
{

class GeneratedInputsManager;

} // namespace inputs

#include "helpers/ordered_relation.hpp"

namespace inputs
{

class GeneratedInputsManager
{
    private:
        static std::map<helpers::OrderedRelation<int>, std::string> adversarialSequencesPrecedencesToWildcardFilename;
        static std::map<helpers::OrderedRelation<int>, std::string> wildcardedAdversarialSequencesPrecedencesToWildcardFilename;
        static std::map<helpers::OrderedRelation<int>, std::string> staticAdversarialSequencesPrecedencesToWildcardFilename;
        static bool loadedAdversarialSequencesPrecedencesToWildcardFilename;

        std::string inputsPath;
        int currentDepth;
        std::map<int, std::string> inputIdWithInputFilename;
        std::map<helpers::OrderedRelation<int>, std::vector<int>> precedenceWithRelatedInputIds;
        std::map<helpers::OrderedRelation<int>, std::set<std::string>> concretizedPrecedences;
        std::optional<std::string> tag;

        static void buildAdversarialSequencesPrecedencesToWildcardFilename();
        static void buildWildcardedAndStaticAdversarialSequencesPrecedencesToWildcardFilename();
        static void staticInitialization();

        void buildInputIdWithInputFilename();
        void buildPrecedencesToInputsIds();
        void buildConcretizedPrecedences();

    public:
        // Default constructor, should be used only in stage 0
        GeneratedInputsManager();
        GeneratedInputsManager(std::string _tag);

        // Constructor for next stages
        GeneratedInputsManager(std::string _inputsPath, int _currentDepth);
        GeneratedInputsManager(std::string _tag, std::string _inputsPath, int _currentDepth);

        void generateWildcardFilesForThisDepth();
        std::vector<std::string> getAllWildcardedAdversarialSequencesFilenames();
        std::vector<std::string> getAllStaticAdversarialSequencesFilenames();
        std::set<helpers::OrderedRelation<int>> getAllOrderedRelationsOfWildcardedAdversarialSequences();
        std::map<helpers::OrderedRelation<int>, std::vector<std::string>> getAllWildcardedAdversarialSequencesFilenamesGroupedByAtMostNPrecedences(int n);
        std::vector<std::vector<std::string>> dispatchWildcardsFilenamesOnNCPUs(int numberOfPrecedences, std::size_t cpus);
};

}

#endif