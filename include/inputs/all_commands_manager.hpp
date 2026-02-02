#ifndef ALL_COMMANDS_MANAGER_HPP
#define ALL_COMMANDS_MANAGER_HPP

#include <string>
#include <exception>

namespace inputs
{

class AllCommandsManagerIndexOutOfBoundsException;
class AllCommandsManagerCommandNotFoundException;
class AllCommandsManager;

} // namespace inputs

#include "helpers/id_bidirectional_map.hpp"

namespace inputs
{

class AllCommandsManagerIndexOutOfBoundsException : public std::exception
{
    private:
        std::string message;

    public:
        AllCommandsManagerIndexOutOfBoundsException(int _i, int _size);

        virtual const char* what() const throw();
};

class AllCommandsManagerCommandNotFoundException : public std::exception
{
    private:
        std::string message;

    public:
        AllCommandsManagerCommandNotFoundException(std::string _command);

        virtual const char* what() const throw();
};

class AllCommandsManager
{
    private:
        std::string allCommandsPath;
        helpers::IdBidirectionalMap<std::string> commands;

        void fillCommands();

    public:
        static bool commandIsStatic(std::string command);

        AllCommandsManager(std::string _allCommandsPath);

        std::string getCommand(int id) const;
        int getID(const std::string& command) const;
        int getNumberOfCommands() const;
};

} // namespace inputs

#endif