#ifndef CBD_INSTANCE_HPP
#define CBD_INSTANCE_HPP

#include <optional>
#include <string>
#include <exception>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

namespace kernel
{

class CBDInstanceUninitializedException;
class CBDInstanceRelatedToolNotFoundException;
class CBDInstanceOutputDirectoryAlreadyCreatedException;
class CBDInstance;

} // namespace kernel

#include "helpers/time/timer.hpp"
#include "inputs/all_commands_manager.hpp"
#include "kernel/cbd_options_manager.hpp"

namespace kernel
{

#pragma region Exceptions

class CBDInstanceUninitializedException : public std::exception
{
    public:
        CBDInstanceUninitializedException();

        virtual const char* what() const throw();
};

class CBDInstanceRelatedToolNotFoundException : public std::exception
{
    private:
        std::string message;

    public:
        CBDInstanceRelatedToolNotFoundException(std::string _relatedTool);
        CBDInstanceRelatedToolNotFoundException(std::string _relatedTool, std::string _reason);

        virtual const char* what() const throw();
};

class CBDInstanceOutputDirectoryAlreadyCreatedException : public std::exception
{
    public:
        CBDInstanceOutputDirectoryAlreadyCreatedException();

        virtual const char* what() const throw();
};

#pragma endregion

class CBDInstance
{
    private:
        static CBDInstance* instance;

        std::string cslFilePath;
        std::string binaryToTestPathRadical;
        std::vector<float> timeToFuzzAWildcard;
        int maxSequencesDepth;
        std::string initialFuzzingSeedsPath;
        std::vector<std::string> binaryToTestArgs;
        int numberOfConcurrentThreads;
        inputs::AllCommandsManager* allCommandsManager;
        const CBDOptionsManager* optionsManager;
        helpers::time::Timer timer;
        bool bestEffortByTimeoutMode;
        std::optional<std::string> initialSeedsPath;
        std::optional<std::string> gotWildcardsPath;
        std::optional<int> iterationNumber;
        std::optional<bool> iterativeModeStrategiesBalancingMode;

        static void checkRelatedTools(std::string toolName, std::string toolPath);

        CBDInstance(std::string _cslFilePath, std::string _binaryToTestPathRadical, std::vector<float> _timeToFuzzAWildcard, int _maxSequencesDepth, std::string _initialFuzzingSeedsPath, std::vector<std::string> _binaryToTestArgs, int _numberOfConcurrentThreads, const CBDOptionsManager* _optionsManager);
        CBDInstance(std::string _cslFilePath, std::string _binaryToTestPathRadical, std::vector<float> _timeToFuzzAWildcard, int _maxSequencesDepth, std::string _initialFuzzingSeedsPath, std::vector<std::string> _binaryToTestArgs, int _numberOfConcurrentThreads, const CBDOptionsManager* _optionsManager, std::string _initialSeedsPath, std::string _gotWildcardsPath, int _iterationNumber, bool _iterativeModeStrategiesBalancingMode);

    public:
        static CBDInstance& initializeInstance(std::string _cslFilePath, std::string _binaryToTestPathRadical, std::vector<float> _timeToFuzzAWildcard, int _maxSequencesDepth, std::string _initialFuzzingSeedsPath, std::vector<std::string> _binaryToTestArgs, int _numberOfConcurrentThreads, const CBDOptionsManager* _optionsManager);
        static CBDInstance& initializeInstance(std::string _cslFilePath, std::string _binaryToTestPathRadical, std::vector<float> _timeToFuzzAWildcard, int _maxSequencesDepth, std::string _initialFuzzingSeedsPath, std::vector<std::string> _binaryToTestArgs, int _numberOfConcurrentThreads, const CBDOptionsManager* _optionsManager, std::string _initialSeedsPath, std::string _gotWildcardsPath, int _iterationNumber, bool _iterativeModeStrategiesBalancingMode);
        static CBDInstance& getInstance();

        CBDInstance(CBDInstance& other) = delete;

        std::string getOriginalBinaryToTestPath() const;
        std::string getAFLOptimizedBinaryToTestPath() const;
        std::string getCmplogBinaryToTestPath() const;
        std::string getCompcovBinaryToTestPath() const;
        void enableAFLSystemOptimizations() const;
        std::string getBinaryToTestDirectory() const;
        std::string getBinaryToTestPath() const;
        void increaseMaxSequenceDepthAndRerunCSLProcess();
        float getTimeToFuzzAWildcard(int stageNumber) const;

        // Getters
        [[deprecated("Use CBDInstance::getTimeToFuzzAWildcard(int currentStageNumber) instead")]] const std::vector<float>& getTimeToFuzzAWildcard() const;
        std::string getInitialFuzzingSeedsPath() const;
        std::vector<std::string> getBinaryToTestArgs() const;
        int getMaxSequencesDepth() const;
        int getNumberOfConcurrentThreads() const;
        const inputs::AllCommandsManager& getAllCommandsManager() const;
        const CBDOptionsManager& getOptionsManager() const;
        const helpers::time::Timer& getTimer() const;
        bool bestEffortByTimeoutModeEnabled() const;
        const std::string& getInitialSeedsPath() const;
        const std::string& getGotWildcardsPath() const;
        int getIterationNumber() const;
        bool getIterativeModeStrategiesBalancingMode() const;

        void operator=(const CBDInstance& other) = delete;

        ~CBDInstance();
};

} // namespace kernel

#endif