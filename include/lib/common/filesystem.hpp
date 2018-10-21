/*
 * filesystem.hpp
 *
 *  Created on: 05/08/18
 *      Author: hello@whatsthecraic.net
 */

#ifndef PERM_VS_SORT_FILESYSTEM_HPP
#define PERM_VS_SORT_FILESYSTEM_HPP

#include <string>

namespace common::filesystem {

/**
 * Change the current w.d. to the specified path, and restore the previous
 * working directory on exit.
 */
class TemporaryWorkingDirectory{
private:
    const std::string m_old_wd; // the working directory to restore on exit
public:

    /**
     * Restore the current working directory on exit, but do not explicitly change it
     */
    TemporaryWorkingDirectory();

    /**
     * Change the working directory to the given path
     * @param path path to the new working directory
     */
    TemporaryWorkingDirectory(const std::string& path);

    /**
     * Restore the previous working directory
     */
    ~TemporaryWorkingDirectory() noexcept(false);
};


/**
 * Get the absolute path for the given (relative) path
 */
std::string absolute_path(const std::string& path);

/**
 * Check whether the given file exists
 */
bool exists(const std::string& path);
bool file_exists(const std::string& path);

/**
 * Check whether the given path exists and it is a directory
 */
bool is_directory(const std::string& path);

/**
 * Retrieve the size, in bytes, of the file in the given path
 */
uint64_t file_size(const std::string& path);

/**
 * Get the path to the current working directory
 */
std::string wd();
std::string working_directory();

/**
 * Retrieve the absolute path to the program executable
 */
std::string path_executable();

/**
 * Retrieve the absolute path to the directory of the executable
 */
std::string directory_executable();

/**
 * Create the given directory. All the parents are also created in the path,
 * emulating the same functionality of `mkdir -pv'
 */
void mkdir(const std::string& path);

} // common::filesystem

#endif //PERM_VS_SORT_FILESYSTEM_HPP
