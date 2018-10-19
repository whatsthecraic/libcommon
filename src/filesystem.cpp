/*
 * filesystem.cpp
 *
 *  Created on: 05/08/18
 *      Author: hello@whatsthecraic.net
 */

#include "filesystem.hpp"

#include <cerrno>
#include <climits> // PATH_MAX
#include <cstdlib> // realpath
#include <cstring>
#include <iostream>
#include <libgen.h> // dirname
#include <sstream>
#include <sys/stat.h>
#include <sys/unistd.h> // chdir, getcwd, getpid

#include "error.hpp"

using namespace std;

namespace common::filesystem {

/******************************************************************************
 *                                                                            *
 * Executable path & basedir                                                  *
 *                                                                            *
 *****************************************************************************/

string path_executable(){
    auto process_id = getpid();
    stringstream ss;
    ss << "/proc/" << process_id << "/exe";
    string strpath = ss.str();

    /*
     * Even though it is recognised as a symlink, unfortunately fs::read_link(path) does
     * not seem to work (yet) with these types of links :/
     */
    constexpr size_t buffer_sz = 512;
    char buffer[buffer_sz];
    int rc = readlink(strpath.c_str(), buffer, buffer_sz);
    if (rc == -1){
        ERROR("Cannot read the link " << strpath);
    }
    strpath = string(buffer, rc);

    return strpath;


}

string directory_executable(){
    string exe = path_executable();

    // FIXME: C++17 <filesystem>
//    fs::path path = strpath;
//    return path.parent_path();

    constexpr size_t buffer_sz = 512;
    char buffer[buffer_sz];
    auto strlength = min(buffer_sz -1, exe.size());
    memcpy(buffer, exe.c_str(), strlength);
    buffer[strlength] = '\0';
    return dirname(buffer);
}



/******************************************************************************
 *                                                                            *
 * Path manipulation                                                          *
 *                                                                            *
 *****************************************************************************/

string absolute_path(const string& path){
    constexpr size_t buffer_sz = PATH_MAX;
    char buffer[buffer_sz];
    char* result = realpath(path.c_str(), buffer);
    if(result == nullptr){
        ERROR("[absolute_path] " << strerror(errno) << " (" << errno << ")");
    }

    return string(result);
}


/******************************************************************************
 *                                                                            *
 * Path checks                                                                *
 *                                                                            *
 *****************************************************************************/

bool exists(const std::string& path){ return file_exists(path); }

bool file_exists(const std::string& path){
    //FIXME: C++17 <filesystem> => fs::exists(path)
    struct stat stat_;
    int rc = stat(path.c_str(), &stat_);
    if (rc == 0)
        return true;
    else if ( errno == ENOENT ){
        return false;
    } else {
        // we don't really know, but whatever the error we are not able to access this file.
        // Treat it like it does not exist.
        return false;
    }
}


/******************************************************************************
 *                                                                            *
 * Working directory                                                          *
 *                                                                            *
 *****************************************************************************/

string wd() {
    constexpr size_t buffer_sz = 512;
    char buffer[buffer_sz];
    char* result = getcwd(buffer, buffer_sz); // getcwd already returns an absolute path
    if(result == nullptr){ ERROR("Cannot retrieve the current working directory: " << strerror(errno) << endl ); }
    return result;
}

string working_directory() { return wd(); }


TemporaryWorkingDirectory::TemporaryWorkingDirectory() : m_old_wd(wd()) { }

TemporaryWorkingDirectory::TemporaryWorkingDirectory(const std::string& path) : TemporaryWorkingDirectory()  {
    int rc = chdir(path.c_str());
    if( rc != 0 ) ERROR("Cannot change the current working directory to " << path << ": " << strerror(errno))
}

TemporaryWorkingDirectory::~TemporaryWorkingDirectory() noexcept(false) {
    if(!m_old_wd.empty() && wd() != m_old_wd){
        int rc = chdir(m_old_wd.c_str());
        if( rc != 0 ) ERROR("Cannot restore the current working directory to " << m_old_wd << ": " << strerror(errno))
    }
}


/******************************************************************************
 *                                                                            *
 * Make directory (mkdir)                                                     *
 *                                                                            *
 *****************************************************************************/
static void mkdir(const char *); // prototype
static void mkdir(const char *path){
    char varpath[PATH_MAX];
    strncpy(varpath, path, PATH_MAX);
    char* parent = dirname(varpath);
    if(!file_exists(string{parent})){
        mkdir(parent);
    }

    // syscall
    int rc = ::mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if(rc != 0){ ERROR("Cannot create the directory " << path << ": " << strerror(errno)); }
}

void mkdir(const std::string& path){
    if(!file_exists(path))
        mkdir(path.c_str());
}

} // namespace common
