#include <filesystem>
#include <fstream>
#include <utility>

#include "inputs/all_commands_manager.hpp"

namespace inputs
{

#pragma region AllCommandsManagerIndexOutOfBoundsException

AllCommandsManagerIndexOutOfBoundsException::AllCommandsManagerIndexOutOfBoundsException(int _i, int _size) :
    message("ERROR : The index " + std::to_string(_i) + " is upper than the size of the commands vector (" + std::to_string(_size) + ").")
{
}

const char* AllCommandsManagerIndexOutOfBoundsException::what() const throw()
{
    return this->message.c_str();
}

#pragma endregion
#pragma region AllCommandsManagerCommandNotFoundException

AllCommandsManagerCommandNotFoundException::AllCommandsManagerCommandNotFoundException(std::string _command) :
    message("ERROR : The command " + _command + " was not found.")
{
}

const char* AllCommandsManagerCommandNotFoundException::what() const throw()
{
    return this->message.c_str();
}

#pragma endregion

void AllCommandsManager::fillCommands()
{
    std::string filenameAsString;
    std::size_t colonPositionInFilename;
    std::size_t dotPositionInFilename;
    std::string commandIDAsString;
    int commandID;
    std::ifstream currentFileIfstream;
    std::string currentCommand;

    for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(this->allCommandsPath))
    {
        // Current command ID management
        filenameAsString = file.path().filename();
        dotPositionInFilename = filenameAsString.find('/');
        colonPositionInFilename = filenameAsString.find(':');
        commandIDAsString = filenameAsString.substr(colonPositionInFilename + 1, filenameAsString.length()).substr(0, dotPositionInFilename);

        commandID = std::stoi(commandIDAsString);

        // Current command management
        currentFileIfstream = std::ifstream(file.path());
        std::getline(currentFileIfstream, currentCommand);

        // Inserting the command into the map
        this->commands.add(std::pair<int, std::string>(commandID, currentCommand));

        // Close and internal ID management
        currentFileIfstream.close();
    }
}

bool AllCommandsManager::commandIsStatic(std::string command)
{
    bool result = true;
    size_t i = 0;

    while ((i < command.size()) && (result == true))
    {
        if (command.at(i) == '*')
        {
            result = false;
        }
        else
        {
            ++i;
        }
    }

    return result;
}

AllCommandsManager::AllCommandsManager(std::string _allCommandsPath) :
    allCommandsPath(_allCommandsPath)
{
    this->fillCommands();
}

std::string AllCommandsManager::getCommand(int id) const
{
    try
    {
        return this->commands.getFromLeft(id);
    }
    catch(const std::exception& e)
    {
        throw AllCommandsManagerIndexOutOfBoundsException(id, this->commands.size());
    }
}

int AllCommandsManager::getID(const std::string& command) const
{
    try
    {
        return this->commands.getFromRight(command);
    }
    catch(const std::exception& e)
    {
        throw AllCommandsManagerCommandNotFoundException(command);
    }
    
}

int AllCommandsManager::getNumberOfCommands() const
{
    return this->commands.size();
}

} // namespace inputs