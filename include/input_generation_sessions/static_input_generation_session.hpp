#ifndef STATIC_INPUT_GENERATION_SESSION
#define STATIC_INPUT_GENERATION_SESSION

#include <string>

namespace input_generation_sessions
{

class StaticInputGenerationSession;

} // namespace input_generation_sessions

#include "input_generation_session.hpp"
#include "stage/cbd_stage.hpp"
#include "helpers/runnables/external_binary_execution.hpp"
#include "inputs/adversarial_sequence.hpp"

namespace input_generation_sessions
{

/**
 * @brief Input Generation Session for inputs that does not need to be fuzzed (Without wildcards)
 * 
 */
class StaticInputGenerationSession final : public InputGenerationSession
{
    private:
        helpers::runnables::ExternalBinaryExecution* testingJob;
        const inputs::AdversarialSequence* adversarialSequence;
        std::string staticSequencePath;

        StaticInputGenerationSession(const stage::CBDStage* _stage, const inputs::AdversarialSequence* _adversarialSequence, std::string _resultsPath, std::string _staticSequencePath);

        void generateTestingJob();

    public:
        void run() override;
        void wait() override;
        void end() override;

        bool wasAlreadyExecuted() const override;
        bool isFinished() const override;

        ~StaticInputGenerationSession();

        friend class InputGenerationSessionFactory;
};

} //namespace input_generation_sessions

#endif