#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace helpers::filesystem
{

/**
 * @brief Get all files (In the form of `std::filesystem::path`) contained in a vector of directories.
 * 
 * @param directories Vector containing directories to list all files.
 * @return std::vector<std::filesystem::path> New vector containing all paths to all files of the directories contained in `directories`.
 */
std::vector<std::filesystem::path> getAllFilesInTheseDirectories(const std::vector<std::string>& directories);

/**
 * @brief Read a file to a new `std::string` which is returned at the end. **This function opens and closes the file**.
 * 
 * @param filePath Path to the file to read.
 * @return std::string Result containing the content of the file.
 */
std::string getFileContent(std::filesystem::path filePath);

/**
 * @brief Get the content of a directory in the form of a vector of `std::filesystem::path`.
 * 
 * @param directoryPath Path to the directory to list the files.
 * @return std::vector<std::filesystem::path> Result containing all paths of files contained in the directory.
 */
std::vector<std::filesystem::path> getDirectoryContent(std::filesystem::path directoryPath);

} // namespace helpers::filesystem

#endif