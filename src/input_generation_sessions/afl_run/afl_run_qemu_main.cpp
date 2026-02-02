#include <string>
#include <vector>

#include "input_generation_sessions/afl_run/afl_run_qemu_main.hpp"
#include "input_generation_sessions/afl_run/afl_run.hpp"
#include "input_generation_sessions/afl_run/fuzzing_timeout.hpp"

namespace input_generation_sessions::afl_run
{

AFLRunQemuMain::AFLRunQemuMain(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs) :
    AFLRun(_fuzzingTimeout, true, _isATextOnlyFuzzingJob, _inputsDirectory, _outputDirectory, true, "main", false, "", _usedWildcardsPath, _binaryToFuzzPath, _binaryToFuzzArgs, false),
    binaryIs32bits(_binaryIs32bits)
{
    this->addEnvironmentVariable("AFL_COMPCOV_LEVEL", "2");
}

AFLRunQemuMain::AFLRunQemuMain(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun) :
    AFLRun(_fuzzingTimeout, true, _isATextOnlyFuzzingJob, _inputsDirectory, _outputDirectory, true, "main", false, "", _usedWildcardsPath, _binaryToFuzzPath, _binaryToFuzzArgs, _isAnExploitAFLRun),
    binaryIs32bits(_binaryIs32bits)
{
    this->addEnvironmentVariable("AFL_COMPCOV_LEVEL", "2");
}

} // namespace input_generation_sessions::afl_run