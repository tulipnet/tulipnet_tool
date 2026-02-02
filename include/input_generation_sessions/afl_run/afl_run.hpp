#ifndef AFL_RUN_HPP
#define AFL_RUN_HPP

#include <string>
#include <vector>
#include <map>

namespace input_generation_sessions::afl_run
{

class AFLRun;

} // namespace input_generation_sessions

#include "helpers/runnables/external_binary_execution.hpp"
#include "helpers/runnables/runnable.hpp"
#include "input_generation_sessions/afl_run/fuzzing_timeout.hpp"

namespace input_generation_sessions::afl_run
{

class AFLRun : public helpers::runnables::Runnable
{
    private:
        FuzzingTimeout fuzzingTimeout;
        bool isAQemuFuzzingJob;
        std::string inputsDirectory;
        std::string outputDirectory;
        bool isAMainFuzzingJob;
        std::string aflInstanceName;
        bool usesACmplogAssociatedBinary;
        std::string cmplogAssociatedBinaryPath;
        std::string usedWildcardsPath;
        std::string usedBinaryPath;
        std::vector<std::string> usedBinaryArgs;
        std::vector<std::string> aflFuzzArgs;
        std::map<std::string, std::string> environment;
        helpers::runnables::ExternalBinaryExecution* fuzzerJob;
        bool isATextOnlyFuzzingJob;
        bool isAnExploitAFLRun;

        void generateAFLFuzzArgs();
        void generateEnvironment();
        void generateFuzzerJob();
        void generateWildcardPathFile() const;

    protected:
        AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isAQemuFuzzingJob, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, bool _usesCmplog, std::string _binaryToFuzzCmplogCompiledBinary, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun);

        void addEnvironmentVariable(std::string name, std::string value);

    public:
        AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs);
        AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun);
        AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, std::string _binaryToFuzzCmplogCompiledBinary, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs);
        AFLRun(FuzzingTimeout _fuzzingTimeout, bool _isATextOnlyFuzzingJob, std::string _inputsDirectory, std::string _outputDirectory, bool _isMain, std::string _aflInstanceName, std::string _binaryToFuzzCmplogCompiledBinary, std::string _usedWildcardsPath, std::string _binaryToFuzzPath, std::vector<std::string> _binaryToFuzzArgs, bool _isAnExploitAFLRun);

        void run() const;
        void wait() const;
        void end() const;

        virtual void run() override;
        virtual void wait() override;
        virtual void end() override;

        std::string getAssociatedCommand() const;
        std::string getAFLInstanceName() const;

        ~AFLRun();
};

} // namespace input_generation_sessions::afl_run

#endif