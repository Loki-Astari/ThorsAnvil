#ifndef THORSANVIL_FILESYSTEM_FILESYSTEM_H
#define THORSANVIL_FILESYSTEM_FILESYSTEM_H

#include "ThorsStorageConfig.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <fstream>

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

#if defined(HEADER_ONLY) && HEADER_ONLY == 1
#include "filesystem.source"
#endif

#endif
