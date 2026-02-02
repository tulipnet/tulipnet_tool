#include <cstdlib>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#ifdef DEBUG
#include <iostream>
#endif

#include "kernel/constants.hpp"
#include "input_generation_sessions/afl_run/afl_run.hpp"
#include "input_generation_sessions/afl_run/fuzzing_timeout.hpp"

namespace input_generation_sessions::afl_run
{

void AFLRun::generateAFLFuzzArgs()
{
    std::vector<std::string> binaryToFuzzArgs = this->usedBinaryArgs;
    char* envCBDUseDictFile;

    if (this->fuzzingTimeout.getTimeoutMode() == FuzzingTimeoutMode::WITH_TIMEOUT)
    {
        this->aflFuzzArgs.push_back("--signal=SIGINT");
        this->aflFuzzArgs.push_back(std::to_string(this->fuzzingTimeout.getTimeToFuzz()));
        this->aflFuzzArgs.push_back("/usr/local/bin/afl-fuzz");
    }
    else if (this->fuzzingTimeout.getTimeoutMode() == FuzzingTimeoutMode::NULL_TIMEOUT)
    {
        // afl-fuzz args
        this->aflFuzzArgs.push_back("-V");
        this->aflFuzzArgs.push_back("0");
    }

    // afl-fuzz args
    if (this->isAQemuFuzzingJob == true)
    {
        this->aflFuzzArgs.push_back("-Q");
    }

    this->aflFuzzArgs.push_back("-i");
    this->aflFuzzArgs.push_back(this->inputsDirectory);
    this->aflFuzzArgs.push_back("-o");
    this->aflFuzzArgs.push_back(this->outputDirectory);

    if (this->isAMainFuzzingJob == true)
    {
        this->aflFuzzArgs.push_back("-M");
    }
    else
    {
        this->aflFuzzArgs.push_back("-S");
    }

    this->aflFuzzArgs.push_back(this->aflInstanceName);

    if (this->isATextOnlyFuzzingJob == true)
    {
        this->aflFuzzArgs.push_back("-a");
        this->aflFuzzArgs.push_back("text");
    }

    if (this->usesACmplogAssociatedBinary == true)
    {
        this->aflFuzzArgs.push_back("-c");
        this->aflFuzzArgs.push_back(this->cmplogAssociatedBinaryPath);
    }
    else if ((this->isAQemuFuzzingJob == true) && (this->isAMainFuzzingJob == true))
    {
        this->aflFuzzArgs.push_back("-c");
        this->aflFuzzArgs.push_back("0");
    }

    if (this->isAnExploitAFLRun == true)
    {
        this->aflFuzzArgs.push_back("-p");
        this->aflFuzzArgs.push_back("exploit");
    }

    envCBDUseDictFile = std::getenv(CBD_DICTIONARY_ENV);

    if (envCBDUseDictFile != NULL)
    {
        this->aflFuzzArgs.push_back("-x");
        this->aflFuzzArgs.push_back(envCBDUseDictFile);
    }

    this->aflFuzzArgs.push_back("--");
    this->aflFuzzArgs.push_back(this->usedBinaryPath);
    this->aflFuzzArgs.insert(this->aflFuzzArgs.end(), binaryToFuzzArgs.begin(), binaryToFuzzArgs.end());
}

void AFLRun::generateEnvironment()
{
    char* cbdPreloadEnv;
    char* aflDebugEnv;
    char* aflIDontCareAboutMissingCrashes;
    char* aflSkipCPUFreq;
    char* aflNoArith;
    char* aflIgnoreTimeouts;
    char* aflPreloadDiscriminateForkserverParent;
    char* aflIgnoreSeedProblems;
    char* aflNoStartupCalibration;
    char* aflTestcacheSize;
    char* aflNoAffinity;

    this->environment.insert(std::pair<std::string, std::string>("AFL_NO_UI", "1"));
    this->environment.insert(std::pair<std::string, std::string>("WILDCARD_PATH", this->usedWildcardsPath));

    cbdPreloadEnv = std::getenv(PRELOAD_ENV);

    if (cbdPreloadEnv != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_PRELOAD", cbdPreloadEnv));
    }

    aflDebugEnv = std::getenv("AFL_DEBUG");

    if (aflDebugEnv != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_DEBUG", aflDebugEnv));
    }

    aflIDontCareAboutMissingCrashes = std::getenv("AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES");

    if (aflIDontCareAboutMissingCrashes != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES", aflIDontCareAboutMissingCrashes));
    }

    aflSkipCPUFreq = std::getenv("AFL_SKIP_CPUFREQ");

    if (aflSkipCPUFreq != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_SKIP_CPUFREQ", aflSkipCPUFreq));
    }

    aflNoArith = std::getenv("AFL_NO_ARITH");

    if (aflNoArith != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_NO_ARITH", aflNoArith));
    }

    aflIgnoreTimeouts = std::getenv("AFL_IGNORE_TIMEOUTS");

    if (aflIgnoreTimeouts != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_IGNORE_TIMEOUTS", aflIgnoreTimeouts));
    }

    aflPreloadDiscriminateForkserverParent = std::getenv("AFL_PRELOAD_DISCRIMINATE_FORKSERVER_PARENT");

    if (aflPreloadDiscriminateForkserverParent != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_PRELOAD_DISCRIMINATE_FORKSERVER_PARENT", aflPreloadDiscriminateForkserverParent));
    }

    aflIgnoreSeedProblems = std::getenv("AFL_IGNORE_SEED_PROBLEMS");

    if (aflIgnoreSeedProblems != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_IGNORE_SEED_PROBLEMS", aflIgnoreSeedProblems));
    }

    aflNoStartupCalibration = std::getenv("AFL_NO_STARTUP_CALIBRATION");

    if (aflNoStartupCalibration != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_NO_STARTUP_CALIBRATION", aflNoStartupCalibration));
    }

    aflTestcacheSize = std::getenv("AFL_TESTCACHE_SIZE");

    if (aflTestcacheSize != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_TESTCACHE_SIZE", aflTestcacheSize));
    }

    aflNoAffinity = std::getenv("AFL_NO_AFFINITY");

    if (aflNoAffinity != NULL)
    {
        this->environment.insert(std::pair<std::string, std::string>("AFL_NO_AFFINITY", aflNoAffinity));
    }
}

void AFLRun::generateFuzzerJob()
{
    std::string resultsPath = this->outputDirectory;
    std::string stdoutFilePath = resultsPath + "/afl_stdout_" + this->aflInstanceName;
    std::string stderrFilePath = resultsPath + "/afl_stderr_" + this->aflInstanceName;

    if (this->fuzzingTimeout.getTimeoutMode() == FuzzingTimeoutMode::WITH_TIMEOUT)
    {
        this->fuzzerJob = new helpers::runnables::ExternalBinaryExecution(true, "/bin/timeout", this->aflFuzzArgs, stdoutFilePath, stderrFilePath, this->environment);
    }
    else
    {
        this->fuzzerJob = new helpers::runnables::ExternalBinaryExecution(true, "/root/ablation_studies/aflpp_vanilla/afl-fuzz", this->aflFuzzArgs, stdoutFilePath, stderrFilePath, this->environment);
    }
}

void AFLRun::generateWildcardPathFile() const
{
    std::ofstream outputFileStream(this->outputDirectory + "/afl_wildcard_path_" + this->aflInstanceName);

    outputFileStream << this->usedWildcardsPath << std::endl;

    outputFileStream.close();
}

AFLRun::AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isAQemuFuzzingJob, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, bool _usesCmplog, std::string _binaryToFuzzCmplogCompiledBinary, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun) :
    fuzzingTimeout(_fuzzingTimeout),
    isAQemuFuzzingJob(_isAQemuFuzzingJob),
    inputsDirectory(_inputsDirectory),
    outputDirectory(_outputDirectory),
    isAMainFuzzingJob(_isMain),
    aflInstanceName(_aflInstanceName),
    usesACmplogAssociatedBinary(_usesCmplog),
    cmplogAssociatedBinaryPath(_binaryToFuzzCmplogCompiledBinary),
    usedWildcardsPath(_usedWildcardsPath),
    usedBinaryPath(_binaryToFuzzPath),
    usedBinaryArgs(_binaryToFuzzArgs),
    aflFuzzArgs(0),
    isATextOnlyFuzzingJob(_isATextOnlyFuzzingJob),
    isAnExploitAFLRun(_isAnExploitAFLRun)
{
    this->generateAFLFuzzArgs();
    this->generateEnvironment();
    this->generateFuzzerJob();
    this->generateWildcardPathFile();
}

void AFLRun::addEnvironmentVariable(std::string name, std::string value)
{
    std::string oldAFLPreloadEnvironmentValue;
    std::string newValue;

    if ((name == "AFL_PRELOAD") && (this->environment.count(name) > 0))
    {
        oldAFLPreloadEnvironmentValue = this->environment.at("AFL_PRELOAD");
        newValue = value + ":" + oldAFLPreloadEnvironmentValue;

        this->environment.erase("AFL_PRELOAD");
        this->environment.insert(std::pair<std::string, std::string>(name, newValue));
    }
    else
    {
        newValue = value;

        this->environment.insert(std::pair<std::string, std::string>(name, value));
    }

    if (this->fuzzerJob != nullptr)
    {
        this->fuzzerJob->addEnvironmentVariable(name, newValue);
    }
}

AFLRun::AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs) :
    AFLRun(_fuzzingTimeout, false, _isATextOnlyFuzzingJob, _inputsDirectory, _outputDirectory, _isMain, _aflInstanceName, false, "", _usedWildcardsPath, _binaryToFuzzPath, _binaryToFuzzArgs, false)
{
}

AFLRun::AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun) :
    AFLRun(_fuzzingTimeout, false, _isATextOnlyFuzzingJob, _inputsDirectory, _outputDirectory, _isMain, _aflInstanceName, false, "", _usedWildcardsPath, _binaryToFuzzPath, _binaryToFuzzArgs, _isAnExploitAFLRun)
{
}

AFLRun::AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, std::string _binaryToFuzzCmplogCompiledBinary, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs) :
    AFLRun(_fuzzingTimeout, false, _isATextOnlyFuzzingJob, _inputsDirectory, _outputDirectory, _isMain, _aflInstanceName, true, _binaryToFuzzCmplogCompiledBinary, _usedWildcardsPath, _binaryToFuzzPath, _binaryToFuzzArgs, false)
{
}

AFLRun::AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, std::string _binaryToFuzzCmplogCompiledBinary, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun) :
    AFLRun(_fuzzingTimeout, false, _isATextOnlyFuzzingJob, _inputsDirectory, _outputDirectory, _isMain, _aflInstanceName, true, _binaryToFuzzCmplogCompiledBinary, _usedWildcardsPath, _binaryToFuzzPath, _binaryToFuzzArgs, _isAnExploitAFLRun)
{
}

void AFLRun::run() const
{
    #ifdef DEBUG

    for (std::pair<std::string, std::string> environmentVariable : this->environment)
    {
        std::cout << environmentVariable.first << "=" << environmentVariable.second << std::endl;
    }

    #endif

    this->fuzzerJob->run();
}

void AFLRun::wait() const
{
    this->fuzzerJob->wait();
}

void AFLRun::end() const
{
    this->fuzzerJob->end();
}

void AFLRun::run()
{
    const AFLRun* thisConst = (const AFLRun*)this;

    thisConst->run();
}

void AFLRun::wait()
{
    const AFLRun* thisConst = (const AFLRun*)this;

    thisConst->wait();
}

void AFLRun::end()
{
    const AFLRun* thisConst = (const AFLRun*)this;

    thisConst->end();
}

std::string AFLRun::getAssociatedCommand() const
{
    return this->fuzzerJob->getThisAsBashCommand();
}

std::string AFLRun::getAFLInstanceName() const
{
    return this->aflInstanceName;
}

AFLRun::~AFLRun()
{
    delete this->fuzzerJob;
}

} // namespace input_generation_sessions::afl_run