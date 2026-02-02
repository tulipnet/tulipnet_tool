#include <filesystem>

#include "input_generation_sessions/input_generation_session.hpp"

namespace input_generation_sessions
{

#pragma region InputGenerationSessionNotEmptyResultDirectoryException

InputGenerationSessionNotEmptyResultDirectoryException::InputGenerationSessionNotEmptyResultDirectoryException(std::string _directory) :
    errorMessage("ERROR : Unable to create the directory \"" + _directory + "\"\n")
{
}

const char* InputGenerationSessionNotEmptyResultDirectoryException::what() const throw()
{
    return this->errorMessage.c_str();
}

#pragma endregion
#pragma region InputGenerationSessionAlreadyExecutedException

InputGenerationSessionAlreadyExecutedException::InputGenerationSessionAlreadyExecutedException(std::string _directory) :
    errorMessage("ERROR : The fuzzing session which have \"" + _directory + "\" as result directory was already launched.\n")
{
}

const char* InputGenerationSessionAlreadyExecutedException::what() const throw()
{
    return this->errorMessage.c_str();
}

#pragma endregion
#pragma region InputGenerationSessionRequiredFileDoesNotExistException

InputGenerationSessionRequiredFileDoesNotExistException::InputGenerationSessionRequiredFileDoesNotExistException(std::string _filePath) :
    errorMessage("ERROR : The following file does not exist : \"" + _filePath + "\".")
{
}

const char* InputGenerationSessionRequiredFileDoesNotExistException::what() const throw()
{
    return this->errorMessage.c_str();
}

#pragma endregion
#pragma region InputGenerationSession

void InputGenerationSession::checkIfFilePathExists(std::string filePath)
{
    int result = access(filePath.c_str(), F_OK);

    if (result == -1)
    {
        throw InputGenerationSessionRequiredFileDoesNotExistException(filePath);
    }
}

void InputGenerationSession::setLaunched()
{
    this->launched = true;
}

InputGenerationSession::InputGenerationSession(std::string _binaryToTestPath, std::vector<std::string> _binaryToTestArgs, std::string _resultsPath, std::string _generatedSeedsPath, int _id, int _cpuCost) :
    binaryToTestPath(_binaryToTestPath),
    binaryToTestArgs(_binaryToTestArgs),
    resultsPath(_resultsPath),
    generatedSeedsPath(_generatedSeedsPath),
    id(_id),
    cpuCost(_cpuCost),
    launched(false)
{
    InputGenerationSession::checkIfFilePathExists(this->getBinaryToTestPath());

    std::filesystem::create_directories(this->resultsPath);
}

std::string InputGenerationSession::getResultsPath() const
{
    return this->resultsPath;
}

std::string InputGenerationSession::getBinaryToTestPath() const
{
    return this->binaryToTestPath;
}

std::vector<std::string> InputGenerationSession::getBinaryToTestArgs() const
{
    return this->binaryToTestArgs;
}

std::string InputGenerationSession::getGeneratedSeedsPath() const
{
    return this->generatedSeedsPath;
}

int InputGenerationSession::getId() const
{
    return this->id;
}

int InputGenerationSession::getCPUCost() const
{
    return this->cpuCost;
}

bool InputGenerationSession::wasLaunched() const
{
    return this->launched;
}

void InputGenerationSession::setGeneratedSeedsPath(std::string _generatedSeedsPath)
{
    this->generatedSeedsPath = _generatedSeedsPath;
}

#pragma endregion

} // namespace helpers::runnables