#ifndef CBD_STAGE_HPP
#define CBD_STAGE_HPP

#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <exception>
#include <vector>

namespace stage
{

class CBDStageUnableToForkException;
class CBDStageUnableToCreateDirectoryException;
class CBDStage;

} // namespace stage

#include "helpers/display/timed_progress_bar.hpp"
#include "kernel/cbd_instance.hpp"
#include "stage/cbd_stage_interface.hpp"
#include "input_generation_sessions/static_input_generation_session.hpp"
#include "inputs/adversarial_sequences_manager.hpp"
#include "input_generation_sessions/fuzzing_input_generation_session.hpp"

namespace stage
{

class CBDStageUnableToCreateDirectoryException : public std::exception
{
    private:
        std::string message;

    public:
        CBDStageUnableToCreateDirectoryException(std::string _directory);

        virtual const char* what() const throw();
};

class CBDStage : public CBDStageInterface
{
    private:
        bool launchedFuzzingRuns;
        bool finishedFuzzingRuns;
        bool reversingWildcardsOfResultsOfFinishedFuzzingRuns;
        bool reversedWildcardsOfResultsOfFinishedFuzzingRuns;
        std::vector<input_generation_sessions::StaticInputGenerationSession*> staticInputGenerationSessions;
        bool launchedStaticInputGenerationSessions;
        bool finishedStaticInputGenerationSessions;
        bool builtSyscalls;
        bool ranSyscallMonitor;
        bool isFinal;
        std::string logTag;
        int endedFuzzingRunsCount;
        inputs::AdversarialSequencesManager* adversarialSequencesManager;
        const CBDStage* previousStage;
        float timeToFuzz;
        std::vector<int> idsOfAdversarialSequencesExclusions;
        helpers::display::TimedProgressBar fuzzingSessionsProgressBar;
        input_generation_sessions::FuzzingInputGenerationSession* fuzzingInputGenerationSession;

        void log(std::string message) const;
        void buildStage();
        void generateSHA256SumsOfGeneratedConcreteInputs() const;

    public:
        CBDStage(const kernel::CBDInstance& _relatedInstance, int _stageNumber);
        CBDStage(const CBDStage& _previousStage, int _stageNumber);
        CBDStage(const CBDStage& _previousStage, int _stageNumber, std::vector<int> _idsOfAdversarialSequencesExclusions);

        virtual void runStage() override;
        virtual void stopStage() override;
        virtual std::string getGeneratedSequencesPath() const override; 
        virtual std::string getStaticSequencesPath() const override;
        virtual std::string getTag() const override;
        virtual bool isFinishing() const override;

        // Getters
        std::vector<input_generation_sessions::InputGenerationSession*> getAllInputGenerationSessions() const;
        const CBDStage* getPreviousStage() const;
        const inputs::AdversarialSequencesManager* getAdversarialSequencesManager() const;
        float getTimeToFuzz() const;
        const std::vector<int> getIdsOfAdversarialSequencesExclusions() const;

        ~CBDStage();
};

} // namespace stage

#endif