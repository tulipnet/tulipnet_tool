#include <cstddef>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "inputs/generated_inputs_manager.hpp"
#include "helpers/filesystem.hpp"
#include "helpers/map.hpp"
#include "helpers/ordered_relation.hpp"
#include "helpers/string.hpp"
#include "helpers/vector.hpp"
#include "input_generation_sessions/afl_run/afl_run_utils.hpp"
#include "kernel/constants.hpp"

namespace inputs
{

std::map<helpers::OrderedRelation<int>, std::string> GeneratedInputsManager::adversarialSequencesPrecedencesToWildcardFilename;
std::map<helpers::OrderedRelation<int>, std::string> GeneratedInputsManager::wildcardedAdversarialSequencesPrecedencesToWildcardFilename;
std::map<helpers::OrderedRelation<int>, std::string> GeneratedInputsManager::staticAdversarialSequencesPrecedencesToWildcardFilename;
bool GeneratedInputsManager::loadedAdversarialSequencesPrecedencesToWildcardFilename = false;

void GeneratedInputsManager::buildAdversarialSequencesPrecedencesToWildcardFilename()
{
    std::string generatedAdversarialSequencesByCSLProcessPath = CSLPROCESS_V2_ADVERSARIAL_SEQUENCES_DIR;
    std::string currentAdversarialSequenceFilename;
    std::string adversarialSequenceContentFieldKey = input_generation_sessions::afl_run::utils::getAWildcardFilenameParameterKey(input_generation_sessions::afl_run::utils::WildcardFilenameParameter::CONTENT);
    helpers::OrderedRelation<int> currentAdversarialSequencePrecedence;

    for (const std::filesystem::directory_entry& adversarialSequenceFile : std::filesystem::directory_iterator(generatedAdversarialSequencesByCSLProcessPath))
    {
        currentAdversarialSequenceFilename = adversarialSequenceFile.path().filename().string();
        currentAdversarialSequencePrecedence = helpers::OrderedRelation<int>(input_generation_sessions::afl_run::utils::getParametersFromWildcardFilename(currentAdversarialSequenceFilename).at(adversarialSequenceContentFieldKey));

        adversarialSequencesPrecedencesToWildcardFilename.insert(std::pair<helpers::OrderedRelation<int>, std::string>(currentAdversarialSequencePrecedence, currentAdversarialSequenceFilename));
    }
}

void GeneratedInputsManager::buildWildcardedAndStaticAdversarialSequencesPrecedencesToWildcardFilename()
{
    std::string currentAdversarialSequencePath;
    std::string currentAdversarialSequenceContent;
    int numberOfStarsInAdversarialSequenceContent;

    for (const std::pair<const helpers::OrderedRelation<int>, std::string>& precedenceToAdversarialSequenceFilename : adversarialSequencesPrecedencesToWildcardFilename)
    {
        currentAdversarialSequencePath = CSLPROCESS_V2_ADVERSARIAL_SEQUENCES_DIR "/" + precedenceToAdversarialSequenceFilename.second;
        currentAdversarialSequenceContent = helpers::filesystem::getFileContent(currentAdversarialSequencePath);

        numberOfStarsInAdversarialSequenceContent = helpers::string::countOccurrencesOfCharacter(currentAdversarialSequenceContent, '*');

        if (numberOfStarsInAdversarialSequenceContent > 0)
        {
            wildcardedAdversarialSequencesPrecedencesToWildcardFilename.insert(precedenceToAdversarialSequenceFilename);
        }
        else if (numberOfStarsInAdversarialSequenceContent == 0)
        {
            staticAdversarialSequencesPrecedencesToWildcardFilename.insert(precedenceToAdversarialSequenceFilename);
        }
    }
}

inline void GeneratedInputsManager::staticInitialization()
{
    if (loadedAdversarialSequencesPrecedencesToWildcardFilename == false)
    {
        buildAdversarialSequencesPrecedencesToWildcardFilename();
        buildWildcardedAndStaticAdversarialSequencesPrecedencesToWildcardFilename();

        loadedAdversarialSequencesPrecedencesToWildcardFilename = true;
    }
}

void GeneratedInputsManager::buildInputIdWithInputFilename()
{
    std::string currentInputFilename;
    int currentInputId;
    input_generation_sessions::afl_run::utils::AFLQueueFilenameParameter idParameter = input_generation_sessions::afl_run::utils::AFLQueueFilenameParameter::ID;
    std::string idParameterAsString = input_generation_sessions::afl_run::utils::getAFLQueueFilenameParameterKey(idParameter);
    int i = 0x10000;

    for (const std::filesystem::directory_entry& inputFile : std::filesystem::directory_iterator(this->inputsPath))
    {
        currentInputFilename = inputFile.path().filename().string();
        currentInputId = std::stoi(input_generation_sessions::afl_run::utils::getParametersFromAFLQueueFilename(inputFile.path().filename().string()).at(idParameterAsString)) + i;

        this->inputIdWithInputFilename.insert(std::pair<int, std::string>(currentInputId, currentInputFilename));

        i += 0x10000;
    }
}

void GeneratedInputsManager::buildPrecedencesToInputsIds()
{
    int currentId;
    std::vector<int> associatedWildcardPrecedences;
    helpers::OrderedRelation<int> associatedWildcardPrecedencesAsOrderedRelation;
    std::string associatedWildcardFilename;

    for (const std::pair<int, std::string> currentIdWithFilename : this->inputIdWithInputFilename)
    {
        currentId = currentIdWithFilename.first;

        associatedWildcardFilename = input_generation_sessions::afl_run::utils::getParametersFromAFLQueueFilename(currentIdWithFilename.second).at(input_generation_sessions::afl_run::utils::getAFLQueueFilenameParameterKey(input_generation_sessions::afl_run::utils::AFLQueueFilenameParameter::WILDCARD));
        associatedWildcardPrecedences = std::vector<int>(input_generation_sessions::afl_run::utils::getParametersFromWildcardFilename(associatedWildcardFilename).at(input_generation_sessions::afl_run::utils::getAWildcardFilenameParameterKey(input_generation_sessions::afl_run::utils::WildcardFilenameParameter::CONTENT)));
        associatedWildcardPrecedencesAsOrderedRelation = helpers::OrderedRelation<int>(associatedWildcardPrecedences);

        associatedWildcardPrecedencesAsOrderedRelation.keepOnlyNFirstElements(this->currentDepth); // We keep only the already fuzzed previous commands

        if (helpers::map::constainsKey(this->precedenceWithRelatedInputIds, associatedWildcardPrecedencesAsOrderedRelation) == true)
        {
            this->precedenceWithRelatedInputIds.at(associatedWildcardPrecedencesAsOrderedRelation).push_back(currentId);
        }
        else
        {
            this->precedenceWithRelatedInputIds.insert(
                std::pair<helpers::OrderedRelation<int>, std::vector<int>>(
                    associatedWildcardPrecedencesAsOrderedRelation,
                    std::vector<int>({currentId})
                )
            );
        }
    }
}

void GeneratedInputsManager::buildConcretizedPrecedences()
{
    std::string currentInputPath;
    std::string currentInputContent;
    std::vector<std::string> splittedCurrentInputContent;
    std::size_t i;
    std::stringstream currentPrecedenceConcretizedValueStream;

    for (const std::pair<const helpers::OrderedRelation<int>, std::vector<int>>& precedencesToRelatedInputsIds : this->precedenceWithRelatedInputIds)
    {
        for (int currentInputId : precedencesToRelatedInputsIds.second)
        {
            currentInputPath = this->inputsPath + "/" + this->inputIdWithInputFilename.at(currentInputId);
            currentInputContent = helpers::filesystem::getFileContent(currentInputPath);
            splittedCurrentInputContent = helpers::string::split(currentInputContent, '\n');

            currentPrecedenceConcretizedValueStream = std::stringstream();

            if ((int)splittedCurrentInputContent.size() >= this->currentDepth)
            {
                for (i = 0; (int)i < this->currentDepth; ++i)
                {
                    currentPrecedenceConcretizedValueStream << splittedCurrentInputContent.at(i);

                    if ((int)(i + 1) != this->currentDepth)
                    {
                        currentPrecedenceConcretizedValueStream << '\n';
                    }
                }
            }

            if (helpers::map::constainsKey(this->concretizedPrecedences, precedencesToRelatedInputsIds.first) == true)
            {
                this->concretizedPrecedences.at(precedencesToRelatedInputsIds.first).insert(currentPrecedenceConcretizedValueStream.str());
            }
            else
            {
                this->concretizedPrecedences.insert(
                    std::pair<helpers::OrderedRelation<int>, std::set<std::string>>(
                        precedencesToRelatedInputsIds.first,
                        std::set<std::string>({currentPrecedenceConcretizedValueStream.str()})
                    )
                );
            }
        }
    }
}

GeneratedInputsManager::GeneratedInputsManager() :
    currentDepth(0),
    tag(std::nullopt)
{
    staticInitialization();
}

GeneratedInputsManager::GeneratedInputsManager(std::string _tag) :
    currentDepth(0),
    tag(_tag)
{
}

GeneratedInputsManager::GeneratedInputsManager(std::string _inputsPath, int _currentDepth) :
    inputsPath(_inputsPath),
    currentDepth(_currentDepth),
    tag(std::nullopt)
{
    staticInitialization();

    this->buildInputIdWithInputFilename();
    this->buildPrecedencesToInputsIds();
    this->buildConcretizedPrecedences();
}

GeneratedInputsManager::GeneratedInputsManager(std::string _tag, std::string _inputsPath, int _currentDepth) :
    inputsPath(_inputsPath),
    currentDepth(_currentDepth),
    tag(_tag)
{
    staticInitialization();

    this->buildInputIdWithInputFilename();
    this->buildPrecedencesToInputsIds();
    this->buildConcretizedPrecedences();
}

void GeneratedInputsManager::generateWildcardFilesForThisDepth()
{
    std::string outputWildcardsDirectory = OUTPUT_DIR_GENERATED_WILDCARDS "/" + std::to_string(this->currentDepth);
    std::string currentAdversarialSequenceFilename;
    std::string currentAdversarialSequenceContent;
    std::string newWildcardContent;
    std::ofstream newWildcardFileOfstream;
    int i = 0;

    std::filesystem::create_directory(outputWildcardsDirectory);

    for (const std::pair<const helpers::OrderedRelation<int>&, const std::set<std::string>&> concretizedPrecedence : this->concretizedPrecedences)
    {
        for (const std::string& concretizedPrecedenceValue : concretizedPrecedence.second)
        {
            for (std::pair<const helpers::OrderedRelation<int>&, const std::string&> candidateWildcard : this->adversarialSequencesPrecedencesToWildcardFilename)
            {
                if (concretizedPrecedence.first << candidateWildcard.first)
                {
                    currentAdversarialSequenceFilename = candidateWildcard.second;
                    currentAdversarialSequenceContent = helpers::filesystem::getFileContent(CSLPROCESS_V2_ADVERSARIAL_SEQUENCES_DIR "/" + currentAdversarialSequenceFilename);

                    if (helpers::string::countOccurrencesOfCharacter(currentAdversarialSequenceContent, '*') > 0)
                    {
                        newWildcardContent = helpers::string::merge(concretizedPrecedenceValue, currentAdversarialSequenceContent, this->currentDepth, '\n');

                        if (this->tag.has_value() == false)
                        {
                            newWildcardFileOfstream = std::ofstream(outputWildcardsDirectory + "/" + currentAdversarialSequenceFilename + ";merged" + std::to_string(this->currentDepth) + "id=" + std::to_string(i));
                        }
                        else
                        {
                            newWildcardFileOfstream = std::ofstream(outputWildcardsDirectory + "/" + currentAdversarialSequenceFilename + ";merged" + std::to_string(this->currentDepth) + "id=" + std::to_string(i) + ";tag=" + this->tag.value());
                        }

                        newWildcardFileOfstream << newWildcardContent;

                        newWildcardFileOfstream.close();
                    }

                    ++i;
                }
            }
        }
    }
}

std::vector<std::string> GeneratedInputsManager::getAllWildcardedAdversarialSequencesFilenames()
{
    std::vector<std::string> result;

    for (const std::pair<const helpers::OrderedRelation<int>, std::string>& currentPair : wildcardedAdversarialSequencesPrecedencesToWildcardFilename)
    {
        result.push_back(currentPair.second);
    }

    return result;
}

std::vector<std::string> GeneratedInputsManager::getAllStaticAdversarialSequencesFilenames()
{
    std::vector<std::string> result;

    for (const std::pair<const helpers::OrderedRelation<int>, std::string>& currentPair : staticAdversarialSequencesPrecedencesToWildcardFilename)
    {
        result.push_back(currentPair.second);
    }

    return result;
}

std::set<helpers::OrderedRelation<int>> GeneratedInputsManager::getAllOrderedRelationsOfWildcardedAdversarialSequences()
{
    return helpers::map::getAllKeys(wildcardedAdversarialSequencesPrecedencesToWildcardFilename);
}

std::map<helpers::OrderedRelation<int>, std::vector<std::string>> GeneratedInputsManager::getAllWildcardedAdversarialSequencesFilenamesGroupedByAtMostNPrecedences(int n)
{
    std::map<helpers::OrderedRelation<int>, std::vector<std::string>> result;
    helpers::OrderedRelation<int> currentOrderedRelation;

    for (const std::pair<const helpers::OrderedRelation<int>, std::string>& currentPair : wildcardedAdversarialSequencesPrecedencesToWildcardFilename)
    {
        currentOrderedRelation = currentPair.first.getThisWithOnlyNFirstElements(n);

        if (helpers::map::constainsKey(result, currentOrderedRelation) == false)
        {
            result.insert(
                std::pair<helpers::OrderedRelation<int>, std::vector<std::string>>(
                    currentOrderedRelation,
                    std::vector<std::string>(
                        { currentPair.second }
                    )
                )
            );
        }
        else
        {
            result.at(currentOrderedRelation).push_back(currentPair.second);
        }
    }

    return result;
}

std::vector<std::vector<std::string>> GeneratedInputsManager::dispatchWildcardsFilenamesOnNCPUs(int numberOfPrecedences, std::size_t cpus)
{
    std::vector<std::vector<std::string>> result;
    std::map<helpers::OrderedRelation<int>, std::vector<std::string>> map = this->getAllWildcardedAdversarialSequencesFilenamesGroupedByAtMostNPrecedences(numberOfPrecedences);
    std::size_t mapSize = map.size();
    std::size_t i;
    std::size_t j = 0;
    std::vector<std::string> currentVector;
    helpers::OrderedRelation<int> currentId;

    if (cpus > mapSize)
    {
        for (i = mapSize; i < cpus; ++i)
        {
            if (map.size() > 0)
            {
                currentId = map.begin()->first;

                for (j = 0; j < 2; ++j)
                {
                    currentVector = helpers::vector::getChunk(map.begin()->second, 2, j);

                    result.push_back(currentVector);
                }

                map.extract(currentId);
            }
        }

        for (const std::pair<const helpers::OrderedRelation<int>, std::vector<std::string>>& pair : map)
        {
            result.push_back(pair.second);
        }
    }
    else if (cpus == mapSize)
    {
        for (const std::pair<const helpers::OrderedRelation<int>, std::vector<std::string>>& pair : map)
        {
            result.push_back(pair.second);
        }
    }
    else
    {
        for (i = 0; i < cpus; ++i)
        {
            j = 0;

            while ((j < mapSize / cpus) && (map.size() > 0))
            {
                currentId = map.begin()->first;

                for (const std::string& currentStr : map.begin()->second)
                {
                    currentVector.push_back(currentStr);
                }

                map.extract(currentId);

                ++j;
            }

            if (currentVector.size() > 0)
            {
                result.push_back(currentVector);
                currentVector = std::vector<std::string>();
            }
        }

        if (map.size() > 0)
        {
            for (const std::pair<const helpers::OrderedRelation<int>, std::vector<std::string>>& pair : map)
            {
                for (const std::string& val : pair.second)
                {
                    currentVector.push_back(val);
                }
            }

            for (const std::string& val : currentVector)
            {
                result.at(result.size() - 1).push_back(val);
            }
        }
    }

    return result;
}

} // namespace inputs