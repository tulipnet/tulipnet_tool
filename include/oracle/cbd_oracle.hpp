#ifndef CBD_ORACLE_HPP
#define CBD_ORACLE_HPP

#include <filesystem>
#include <string>
#include <exception>
#include <vector>

namespace oracle
{

class CBDOracleUnableToCreateDirectoryException;
class CBDOracleNotAlreadyExecutedException;
class CBDOracle;

} // namespace oracle

#include "stage/cbd_stage_interface.hpp"

namespace oracle
{

class CBDOracleUnableToCreateDirectoryException : public std::exception
{
    private:
        std::string message;

    public:
        CBDOracleUnableToCreateDirectoryException(std::string _directory);

        virtual const char* what() const throw();
};

class CBDOracleNotAlreadyExecutedException : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class CBDOracle
{
    private:
        const stage::CBDStageInterface& relatedStage;
        std::filesystem::path inputsDir;
        std::string tag;
        bool builtSyscalls;
        std::string relatedSyscallMonitorOutputDir;
        std::string potentialBackdoorsPath;
        std::vector<int> relatedAdversarialSequencesIdsInRelatedStageThatCorrespondToFoundBackdoors;
        std::optional<std::string> alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath;

        void printOracleMessage(std::string message);
        void buildSyscalls();
        void runSyscallMonitor();
        void runCSLAutomaton();
        void findStrangeCommandSequences();
        void removeAlreadyProcessedConcreteInputs() const;

    public:
        CBDOracle(const stage::CBDStageInterface& _relatedStage, std::filesystem::path _inputsDir, std::string _tag);
        CBDOracle(const stage::CBDStageInterface& _relatedStage, std::filesystem::path _inputsDir, std::string _tag, std::optional<std::string> _alreadyProcessedByOracleSHA256SumsOfConcreteInputsFilePath);

        void runOracle();
        std::vector<std::string> getFoundBackdoors() const;

        // Getters
        std::string getTag() const;
        const stage::CBDStageInterface& getRelatedStage() const;
        const std::vector<int>& getRelatedAdversarialSequencesIdsInRelatedStageThatCorrespondToFoundBackdoors() const;
};

} // namespace oracle

#endif