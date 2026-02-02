namespace input_generation_sessions::afl_run
{

class AFLRunQemuQasan;

}

#include "input_generation_sessions/afl_run/afl_run.hpp"

namespace input_generation_sessions::afl_run
{

class AFLRunQemuQasan : public AFLRun
{
    private:
        bool binaryIs32bits;

    public:
        AFLRunQemuQasan(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs);
        AFLRunQemuQasan(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun);
};

}