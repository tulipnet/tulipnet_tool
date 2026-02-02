#include <cstdarg>
#include <cstddef>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

#include "kernel/cbd_options_manager.hpp"
#include "helpers/enum_check.hpp"
#include "helpers/display/terminal.hpp"

namespace kernel
{

#pragma region CBDOptionsManagerOptionNotDefinedException

CBDOptionsManagerOptionNotDefinedException::CBDOptionsManagerOptionNotDefinedException(char _optionCharacter) :
    message("ERROR : The option '" + std::string(1, _optionCharacter) + "' is not recognized")
{
}

const char* CBDOptionsManagerOptionNotDefinedException::what() const throw()
{
    return this->message.c_str();
}

#pragma endregion
#pragma region CBDOptionsManagerInconsistentOptionsException

CBDOptionsManagerInconsistentOptionsException::CBDOptionsManagerInconsistentOptionsException(char _optionCharacterA, char _optionCharacterB, Problem _problem)
{
    switch (_problem)
    {
        case Problem::INCOMPATIBLE_OPTIONS:
            this->message = "ERROR : The options '" + std::string(1, _optionCharacterA) + "' and '" + std::string(1, _optionCharacterB) + "' are incompatibles";
            break;

        case Problem::OPTION_A_IS_NOT_DEFINED:
            this->message = "ERROR : The options '" + std::string(1, _optionCharacterA) + "' in not set, but the option '" + std::string(1, _optionCharacterB) + "' is set";
    }
}

const char* CBDOptionsManagerInconsistentOptionsException::what() const throw()
{
    return this->message.c_str();
}

#pragma endregion

void CBDOptionsManager::buildOptions(std::string options)
{
    const char* optionsCStr = options.c_str();
    std::size_t optionsSize = options.size();
    std::size_t i;
    std::pair<bool, std::optional<std::pair<CBDOptionsManager::Options, CBDOptionsManager::Options>>> check;

    for (i = 0; i < optionsSize; ++i)
    {
        if (helpers::EnumCheck<CBDOptionsManager::Options,
            CBDOptionsManager::Options::FUZZING_MODE_BINARY_ONLY,
            CBDOptionsManager::Options::TARGET_IS_32_BITS,
            CBDOptionsManager::Options::FUZZING_MODE_CMPLOG_ONLY,
            CBDOptionsManager::Options::FUZZING_MODE_COMPCOV_ONLY,
            CBDOptionsManager::Options::FUZZING_MODE_DEGRADED,
            CBDOptionsManager::Options::FUZZING_MODE_FULL,
            CBDOptionsManager::Options::FUZZING_OPTION_TEXT_ONLY,
            CBDOptionsManager::Options::SUPPOSED_THAT_AFL_SYSTEM_OPTIMIZATIONS_ARE_ALREADY_ENABLED,
            CBDOptionsManager::Options::CUSTOM_FUZZING_TIME_PER_FUZZING_SESSION,
            CBDOptionsManager::Options::ORACLE_TESTING_AFTER_EACH_STAGE,
            CBDOptionsManager::Options::BEST_EFFORT_BY_TIMEOUT,
            CBDOptionsManager::Options::FUZZING_MODE_STANDARD_ONLY,
            CBDOptionsManager::Options::ITERATIVE_MODE,
            CBDOptionsManager::Options::ITERATIVE_MODE_STRATEGIES_BALANCING,
            CBDOptionsManager::Options::DISCARD_TRIVIAL_ADVERSARIAL_SEQUENCES,
            CBDOptionsManager::Options::ORACLE_PARALLELIZATION_BY_EXECUTING_STATIC_SEQUENCES_ORACLE_DURING_LAST_FUZZING_STAGE,
            CBDOptionsManager::Options::REMOVE_ALTREADY_TREATED_CONCRETE_INPUTS_BEFORE_ORACLE>::isValid(optionsCStr[i]) == true)
        {
            this->definedOptions.push_back(CBDOptionsManager::Options(optionsCStr[i]));
        }
        else if (optionsCStr[i] != '-')
        {
            throw CBDOptionsManagerOptionNotDefinedException(optionsCStr[i]);
        }
    }

    check = this->checkOptions();

    if (check.first == false)
    {
        if ((check.second->first == CBDOptionsManager::Options::FUZZING_MODE_BINARY_ONLY) && (check.second->second == CBDOptionsManager::Options::TARGET_IS_32_BITS))
        {
            throw CBDOptionsManagerInconsistentOptionsException((char)check.second->first, (char)check.second->second, CBDOptionsManagerInconsistentOptionsException::Problem::OPTION_A_IS_NOT_DEFINED);
        }
        if ((check.second->first == CBDOptionsManager::Options::ITERATIVE_MODE) && (check.second->second == CBDOptionsManager::Options::REMOVE_ALTREADY_TREATED_CONCRETE_INPUTS_BEFORE_ORACLE))
        {
            throw CBDOptionsManagerInconsistentOptionsException((char)check.second->first, (char)check.second->second, CBDOptionsManagerInconsistentOptionsException::Problem::OPTION_A_IS_NOT_DEFINED);
        }
        else
        {
            throw CBDOptionsManagerInconsistentOptionsException((char)check.second->first, (char)check.second->second, CBDOptionsManagerInconsistentOptionsException::Problem::INCOMPATIBLE_OPTIONS);
        }
    }
}

std::pair<bool, std::optional<std::pair<CBDOptionsManager::Options, CBDOptionsManager::Options>>> CBDOptionsManager::checkOptions() const
{
    using resultSecondMemberType = std::pair<CBDOptionsManager::Options, CBDOptionsManager::Options>;
    using resultType = std::pair<bool, std::optional<resultSecondMemberType>>;

    resultType result = resultType(true, std::nullopt);
    std::size_t i = 0;
    std::size_t numberOfDefinedOptions = this->definedOptions.size();
    CBDOptionsManager::Options firstOption;
    CBDOptionsManager::Options secondOption;
    bool foundFirstOption = false;
    bool foundSecondOption = false;
    CBDOptionsManager::Options currentOption;

    while ((i < numberOfDefinedOptions) && (foundSecondOption == false))
    {
        currentOption = this->definedOptions.at(i);

        // Fuzzing-related options management
        switch (currentOption)
        {
            case CBDOptionsManager::Options::FUZZING_MODE_BINARY_ONLY:
            case CBDOptionsManager::Options::FUZZING_MODE_DEGRADED:
            case CBDOptionsManager::Options::FUZZING_MODE_FULL:
            case CBDOptionsManager::Options::FUZZING_MODE_CMPLOG_ONLY:
            case CBDOptionsManager::Options::FUZZING_MODE_COMPCOV_ONLY:
            case CBDOptionsManager::Options::FUZZING_MODE_STANDARD_ONLY:
                if (foundFirstOption == false)
                {
                    foundFirstOption = true;
                    firstOption = currentOption;
                }
                else
                {
                    foundSecondOption = true;
                    secondOption = currentOption;
                }

            default:
                break;
        }

        ++i;
    }

    if ((foundFirstOption == true) && (foundSecondOption == true))
    {
        result = resultType(false, resultSecondMemberType(firstOption, secondOption));
    }
    else if ((this->isDefined(CBDOptionsManager::Options::FUZZING_MODE_BINARY_ONLY) == false) && (this->isDefined(CBDOptionsManager::Options::TARGET_IS_32_BITS) == true)) // We verify if we use "-l", "-b" is set
    {
        result = resultType(false, resultSecondMemberType(CBDOptionsManager::Options::FUZZING_MODE_BINARY_ONLY, CBDOptionsManager::Options::TARGET_IS_32_BITS));
    }
    else if ((this->isDefined(Options::ITERATIVE_MODE) == false) && (this->isDefined(Options::ITERATIVE_MODE_STRATEGIES_BALANCING) == true))
    {
        result = resultType(false, resultSecondMemberType(Options::ITERATIVE_MODE, Options::ITERATIVE_MODE_STRATEGIES_BALANCING));
    }
    else if ((this->isDefined(Options::ORACLE_TESTING_AFTER_EACH_STAGE) == true) && (this->isDefined(Options::ORACLE_PARALLELIZATION_BY_EXECUTING_STATIC_SEQUENCES_ORACLE_DURING_LAST_FUZZING_STAGE) == true))
    {
        result = resultType(false, resultSecondMemberType(Options::ORACLE_TESTING_AFTER_EACH_STAGE, Options::ORACLE_PARALLELIZATION_BY_EXECUTING_STATIC_SEQUENCES_ORACLE_DURING_LAST_FUZZING_STAGE));
    }
    else if ((this->isDefined(Options::ITERATIVE_MODE) == false) && (this->isDefined(Options::REMOVE_ALTREADY_TREATED_CONCRETE_INPUTS_BEFORE_ORACLE) == true))
    {
        result = resultType(false, resultSecondMemberType(Options::ITERATIVE_MODE, Options::REMOVE_ALTREADY_TREATED_CONCRETE_INPUTS_BEFORE_ORACLE));
    }

    return result;
}

std::string CBDOptionsManager::description()
{
    std::stringstream result;

    result << "-b : Binary only mode => Use only the binary <binary_to_fuzz_RADICAL>.ori. Permits to run CBD when the source code of the target is not available." << std::endl;
    result << "    -l : With '-b', legacy mode => Indicate that the <binary_to_fuzz_RADICAL>.ori is a 32 bits binary" << std::endl;
    result << "-d : Direct run => Suppose that the AFL optimizations are already enabled. Useful to avoid calling sudo." << std::endl;
    result << "-f : Full fuzzing mode => Fuzz with 1 main fuzzer, 1 cmplog and 3 compcov AFL++ instances" << std::endl;
    result << "-s : Partial fuzzing mode => Fuzz with 1 main fuzzer, 1 cmplog and 1 compcov AFL++ instances" << std::endl;
    result << "-c : Cmplog minimal fuzzing mode => Fuzz with only 1 main cmplog AFL++ instance" << std::endl;
    result << "-C : Compcov minimal fuzzing mode => Fuzz with only 1 main compcov AFL++ instance" << std::endl;
    result << "-S : Standard minimal fuzzing mode => Fuzz with only 1 main standard AFL++ instance" << std::endl;
    result << "-t : Text-only fuzzing => The fuzzer will only generate text" << std::endl;
    result << "-o : Oracle testing after each stage => The oracle will test generated sequences after each stage, and then filter out potential backdoors for next sessions." << std::endl;
    result << "-T : Custom fuzzing time per fuzzing session -> Now, the parameter \"time_to_fuzz_a_wildcard_in_s\" has the form \"time_to_fuzz_a_wildcard_in_s_for_fuzzing_session_1,time_to_fuzz_a_wildcard_in_s_for_fuzzing_session_2,...\", so there is the same number of values as \"max_command_sequences_depth\"" << std::endl;
    result << "-B : Best effort fuzzing by timeout -> Now, the parameter \"max_command_sequences_depth\" correspond to the fuzzing timeout. When it is reached, the current fuzzing session is stopped, and the oracle testing tries to discover backdoors in performed fuzzing sessions" << std::endl;
    result << "    Could be used with -T to provide a custom distribution of fuzzing time per session. By default, it is uniform, but it could be a good idea to provide longer fuzzing time at the begining, and less at the end. The last value of the vector is the fuzzing time for each remaining fuzzing sessions" << std::endl;
    result << "-i' : Iterative mode => Instead of running CBD sequentially (Phase 1, then phase 2), CBD will be re-runt n times and seeds got from iteration n - 1 will be reused by the fuzzer" << std::endl;
    result << helpers::display::Colors::CYAN << "    After <number_of_current_threads>, there are extra args : <inputs_path> <wildcards_path> <iteration_number>" << helpers::display::Colors::NEUTRAL << std::endl;
    result << helpers::display::Colors::RED_BOLD << "    ATTENTION " << helpers::display::Colors::MAGENTA_BOLD << "You should use the wrapper script \"cbd-iterative\" instead of using -i by yourself !" << helpers::display::Colors::NEUTRAL << std::endl;
    result << "    -e : With '-i', alternate between explore and exploit mode across iterations (It 1 : Explore, It 2 : Exploit, It 3 : Explore, ...)" << std::endl;
    result << "    -r : Remove already processed concrete inputs before oracle testing (Generated and static sequences)" << std::endl;
    result << helpers::display::Colors::YELLOW_BOLD << "        WARNING " << helpers::display::Colors::MAGENTA_BOLD << "Backdoors found in previous iterations may not be found during this iteration" << helpers::display::Colors::NEUTRAL << std::endl;
    result << "-D : Discard trivial (i.e. sequences without wildcards) generated by the CSL" << std::endl;
    result << "-p : Execute static sequences oracle during the last fuzzing stage" << std::endl;
    result << helpers::display::Colors::YELLOW_BOLD << "        WARNING " << helpers::display::Colors::MAGENTA_BOLD << "Expect having at least one more free CPU than CPUs used for fuzzing (Passed as parameter to CBD)" << helpers::display::Colors::NEUTRAL << std::endl;
    result << "    It is not compatible with -o" << std::endl;

    return result.str();
}

CBDOptionsManager::CBDOptionsManager()
{
    this->definedOptions.push_back(CBDOptionsManager::Options::FUZZING_MODE_FULL);
}

CBDOptionsManager::CBDOptionsManager(std::string _options)
{
    this->buildOptions(_options);
}

bool CBDOptionsManager::isDefined(CBDOptionsManager::Options option) const
{
    bool result = false;
    std::size_t i = 0;

    while ((i < this->definedOptions.size()) && (result == false))
    {
        if (this->definedOptions.at(i) == option)
        {
            result = true;
        }
        else
        {
            ++i;
        }
    }

    return result;
}

const std::vector<CBDOptionsManager::Options>& CBDOptionsManager::getDefinedOptions() const
{
    return this->definedOptions;
}

bool CBDOptionsManager::atLeastOneOfTheseOptionsIsDefined(int numberOfOptions, CBDOptionsManager::Options options...) const
{
    std::va_list args;
    int i = 0;
    bool result = false;
    CBDOptionsManager::Options currentArgOption = options;

    va_start(args, options);

    while ((i < numberOfOptions) && (result == false))
    {
        if (this->isDefined(currentArgOption) == true)
        {
            result = true;
        }
        else
        {
            ++i;

            if (i < numberOfOptions)
            {
                currentArgOption = va_arg(args, CBDOptionsManager::Options);
            }
        }
    }

    va_end(args);

    return result;
}

} // namespace kernel