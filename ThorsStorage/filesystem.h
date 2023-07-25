#ifndef THORSANVIL_FILESYSTEM_FILESYSTEM_H
#define THORSANVIL_FILESYSTEM_FILESYSTEM_H

#include "ThorsStorageConfig.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <fstream>

// We haven't checked which filesystem to include yet
#ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL

// Check for feature test macro for <filesystem>
#   if defined(__cpp_lib_filesystem)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0

// Check for feature test macro for <experimental/filesystem>
#   elif defined(__cpp_lib_experimental_filesystem)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// We can't check if headers exist...
// Let's assume experimental to be safe
#   elif !defined(__has_include)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Check if the header "<filesystem>" exists
#   elif __has_include(<filesystem>)

// If we're compiling on Visual Studio and are not compiling with C++17, we need to use experimental
#       ifdef _MSC_VER

// Check and include header that defines "_HAS_CXX17"
#           if __has_include(<yvals_core.h>)
#               include <yvals_core.h>

// Check for enabled C++17 support
#               if defined(_HAS_CXX17) && _HAS_CXX17
// We're using C++17, so let's use the normal version
#                   define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#               endif
#           endif

// If the marco isn't defined yet, that means any of the other VS specific checks failed, so we need to use experimental
#           ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
#               define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
#           endif

// Not on Visual Studio. Let's use the normal version
#       else // #ifdef _MSC_VER
#           define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#       endif

// Check if the header "<filesystem>" exists
#   elif __has_include(<experimental/filesystem>)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Fail if neither header is available with a nice error message
#   else
#       error Could not find system header "<filesystem>" or "<experimental/filesystem>"
#   endif

// We priously determined that we need the exprimental version


#if INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
// Include it
#include <experimental/filesystem>
namespace std
{
    namespace filesystem = experimental::filesystem;
}

#else
#include <filesystem>
#endif

#endif

// See: https://codereview.stackexchange.com/questions/81922/macro-to-build-type-declaration
// For details about the Traits type and how it is built.

namespace ThorsAnvil
{
    namespace FileSystem
    {

    using iostate   = std::ios_base::iostate;
    static constexpr iostate const& goodbit   = std::ios_base::goodbit;
    static constexpr iostate const& badbit    = std::ios_base::badbit;
    static constexpr iostate const& failbit   = std::ios_base::failbit;
    static constexpr iostate const& eofbit    = std::ios_base::eofbit;

    using openmode = std::ios_base::openmode;
    static constexpr openmode const& app      = std::ios_base::app;
    static constexpr openmode const& binary   = std::ios_base::binary;
    static constexpr openmode const& in       = std::ios_base::in;
    static constexpr openmode const& out      = std::ios_base::out;
    static constexpr openmode const& trunc    = std::ios_base::trunc;
    static constexpr openmode const& ate      = std::ios_base::ate;

    // File System Functionality
    struct FileSystem
    {
        enum DirResult {DirAlreadyExists, DirCreated, DirFailedToCreate};
        static DirResult makeDirectory(std::string const& path, mode_t permissions);
        static bool      isFileOpenable(std::string const& path, openmode mode);
        static bool      removeFileOrDirectory(std::string const& path);
    };

    }
}

#if defined(THORS_STORAGE_HEADER_ONLY) && THORS_STORAGE_HEADER_ONLY == 1
#include "filesystem.source"
#endif

#endif
