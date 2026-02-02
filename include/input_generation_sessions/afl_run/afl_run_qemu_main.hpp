#ifndef AFL_RUN_QEMU_MAIN_HPP
#define AFL_RUN_QEMU_MAIN_HPP

#include <vector>
#include <string>

namespace input_generation_sessions::afl_run
{

class AFLRunQemuMain;

} // namespace input_generation_sessions::afl_run

#include "input_generation_sessions/afl_run/afl_run.hpp"
#include "input_generation_sessions/afl_run/fuzzing_timeout.hpp"

namespace input_generation_sessions::afl_run
{

class AFLRunQemuMain : public AFLRun
{
    private:
        bool binaryIs32bits;

    public:
        AFLRunQemuMain(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs);
        AFLRunQemuMain(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun);
};

} // namespace input_generation_sessions::afl_run

#endif