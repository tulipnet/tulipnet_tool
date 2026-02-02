#ifndef AFL_RUN_QEMU_COMPCOV
#define AFL_RUN_QEMU_COMPCOV

#include <vector>
#include <string>

namespace input_generation_sessions::afl_run
{

class AFLRunQemuCompcov;

}

#include "input_generation_sessions/afl_run/afl_run.hpp"

namespace input_generation_sessions::afl_run
{

class AFLRunQemuCompcov : public AFLRun
{
    private:
        bool binaryIs32bits;

    public:
        AFLRunQemuCompcov(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs);
        AFLRunQemuCompcov(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun);
};

}

#endif