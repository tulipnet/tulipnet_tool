#ifndef FUZZING_INPUT_GENERATION_SESSION
#define FUZZING_INPUT_GENERATION_SESSION

#include "inputs/generated_inputs_manager.hpp"
#include <vector>
#include <string>
#include <exception>

namespace input_generation_sessions
{

class NotFinishedFuzzingSessionException;
class FuzzingInputGenerationSession;

} // namespace input_generation_sessions

#include "input_generation_sessions/input_generation_session.hpp"
#include "input_generation_sessions/afl_run/afl_run.hpp"
#include "stage/cbd_stage.hpp"

namespace input_generation_sessions
{

/**
 * @brief Exception thrown when an operation that needs to have an ended fuzzing session was performed on a running fuzzing session.
 * 
 */
class NotFinishedFuzzingSessionException : public std::exception
{
    public:
        NotFinishedFuzzingSessionException();

        virtual const char* what() const throw();
};

/**
 * @brief InputGenerationSession based on fuzzing
 * 
 */
class FuzzingInputGenerationSession : public InputGenerationSession
{
    private:
        const stage::CBDStage* relatedStage;
        std::vector<afl_run::AFLRun*> aflRuns;
        std::string initialSeedsPath;
        bool finished;
        std::vector<std::filesystem::path> adversarialSequences;
        std::string generatedWildcardedAdversarialSequencePath;
        std::optional<inputs::GeneratedInputsManager> generatedInputsManager;

        void prepareFuzzingSession() const;
        void concretizeWildcards() const;
        void generateAFLRuns();
        void forAllAFLRuns(void (*action)(const afl_run::AFLRun* _aflRun)) const;
        void generateWildcardsForThisSession();
        void generateStaticSequencesForThisSession();

    public:
        /**
         * @brief Construct a new Fuzzing Input Generation Session object
         * 
         * @param _relatedStage Related CBDStage
         * @param _adversarialSequences Adversarial sequences used by this FuzzingInputGenerationSession
         */
        FuzzingInputGenerationSession(const stage::CBDStage* _relatedStage, std::vector<std::filesystem::path> _adversarialSequences);

        virtual void run() override;
        virtual void end() override;
        virtual void wait() override;
        virtual bool wasAlreadyExecuted() const override;
        virtual bool isFinished() const override;

        bool applyWildcardsOfResultsOfFinishedFuzzingRuns() const;

        ~FuzzingInputGenerationSession();
};

} // namespace input_generation_sessions

#endif