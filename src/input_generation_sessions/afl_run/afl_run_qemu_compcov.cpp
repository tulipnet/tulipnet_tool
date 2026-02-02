#include "input_generation_sessions/afl_run/afl_run_qemu_compcov.hpp"

namespace input_generation_sessions::afl_run
{

AFLRunQemuCompcov::AFLRunQemuCompcov(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs) :
    AFLRun(_fuzzingTimeout, true, _isATextOnlyFuzzingJob, _inputsDirectory, _outputDirectory, false, "sec_compcov", false, "", _usedWildcardsPath, _binaryToFuzzPath, _binaryToFuzzArgs, false),
    binaryIs32bits(_binaryIs32bits)
{
    this->addEnvironmentVariable("AFL_PRELOAD", "/usr/local/lib/afl/libcompcov.so");
    this->addEnvironmentVariable("AFL_COMPCOV_LEVEL", "2");
}

AFLRunQemuCompcov::AFLRunQemuCompcov(FuzzingTimeout _fuzzingTimeout, bool _binaryIs32bits, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun) :
    AFLRun(_fuzzingTimeout, true, _isATextOnlyFuzzingJob, _inputsDirectory, _outputDirectory, false, "sec_compcov", false, "", _usedWildcardsPath, _binaryToFuzzPath, _binaryToFuzzArgs, _isAnExploitAFLRun),
    binaryIs32bits(_binaryIs32bits)
{
    this->addEnvironmentVariable("AFL_PRELOAD", "/usr/local/lib/afl/libcompcov.so");
    this->addEnvironmentVariable("AFL_COMPCOV_LEVEL", "2");
}

}