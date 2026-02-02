#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "helpers/filesystem.hpp"

namespace helpers::filesystem
{

std::vector<std::filesystem::path> getAllFilesInTheseDirectories(const std::vector<std::string>& directories)
{
    std::vector<std::filesystem::path> result;
    std::filesystem::directory_iterator currentDirectoryIterator;

    for (const std::string& directory : directories)
    {
        currentDirectoryIterator = std::filesystem::directory_iterator(directory);

        for (const std::filesystem::directory_entry& entry : currentDirectoryIterator)
        {
            result.push_back(entry);
        }
    }

    return result;
}

std::string getFileContent(std::filesystem::path filePath)
{
    std::stringstream result;
    std::ifstream fileStream(filePath);
    std::string lineBuffer;

    while (std::getline(fileStream, lineBuffer).fail() == false)
    {
        result << lineBuffer << std::endl;
    }

    fileStream.close();

    return result.str();
}

std::vector<std::filesystem::path> getDirectoryContent(std::filesystem::path directoryPath)
{
    std::vector<std::filesystem::path> result;

    for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(directoryPath))
    {
        result.push_back(directoryEntry.path());
    }

    return result;
}

} // namespace helpers::filesystem