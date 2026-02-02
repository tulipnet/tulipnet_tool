#ifndef CBD_OPTIONS_MANAGER
#define CBD_OPTIONS_MANAGER

#include <string>
#include <vector>
#include <exception>
#include <utility>
#include <optional>

namespace kernel
{

class CBDOptionsManagerOptionNotDefined;
class CBDOptionsManagerInconsistentOptions;
class CBDOptionsManager;

class CBDOptionsManagerOptionNotDefinedException : public std::exception
{
    private:
        std::string message;

    public:
        CBDOptionsManagerOptionNotDefinedException(char _optionCharacter);

        virtual const char* what() const throw() override;
};

class CBDOptionsManagerInconsistentOptionsException : public std::exception
{
    public:
        enum class Problem
        {
            INCOMPATIBLE_OPTIONS,
            OPTION_A_IS_NOT_DEFINED
        };

    private:
        std::string message;

    public:
        CBDOptionsManagerInconsistentOptionsException(char _optionCharacterA, char _optionCharacterB, Problem _problem);

        virtual const char* what() const throw() override;
};

class CBDOptionsManager
{
    public:
        enum class Options : char
        {
            FUZZING_MODE_FULL = 'f',
            FUZZING_MODE_BINARY_ONLY = 'b',
            TARGET_IS_32_BITS = 'l',
            FUZZING_MODE_DEGRADED = 's',
            FUZZING_MODE_CMPLOG_ONLY = 'c',
            FUZZING_MODE_COMPCOV_ONLY = 'C',
            FUZZING_MODE_STANDARD_ONLY = 'S',
            SUPPOSED_THAT_AFL_SYSTEM_OPTIMIZATIONS_ARE_ALREADY_ENABLED = 'd',
            FUZZING_OPTION_TEXT_ONLY = 't',
            CUSTOM_FUZZING_TIME_PER_FUZZING_SESSION = 'T',
            ORACLE_TESTING_AFTER_EACH_STAGE = 'o',
            BEST_EFFORT_BY_TIMEOUT = 'B',
            ITERATIVE_MODE = 'i',
            ITERATIVE_MODE_STRATEGIES_BALANCING = 'e',
            DISCARD_TRIVIAL_ADVERSARIAL_SEQUENCES = 'D',
            ORACLE_PARALLELIZATION_BY_EXECUTING_STATIC_SEQUENCES_ORACLE_DURING_LAST_FUZZING_STAGE = 'p',
            REMOVE_ALTREADY_TREATED_CONCRETE_INPUTS_BEFORE_ORACLE = 'r'
        };

    private:
        std::vector<CBDOptionsManager::Options> definedOptions;

        void buildOptions(std::string options);
        std::pair<bool, std::optional<std::pair<CBDOptionsManager::Options, CBDOptionsManager::Options>>> checkOptions() const;

    public:
        static std::string description();

        CBDOptionsManager();
        CBDOptionsManager(std::string _options);

        bool isDefined(CBDOptionsManager::Options option) const;
        const std::vector<CBDOptionsManager::Options>& getDefinedOptions() const;
        bool atLeastOneOfTheseOptionsIsDefined(int numberOfOptions, CBDOptionsManager::Options options...) const;
};

} // namespace kernel

#endif