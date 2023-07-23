#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_FILE_H
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_FILE_H

#include "ThorsStorageConfig.h"
#include "filesystem.h"
#include <ThorSerialize/Traits.h>
#include <string>
#include <tuple>

// See: https://codereview.stackexchange.com/questions/81922/macro-to-build-type-declaration
// For details about the Traits type and how it is built.

namespace ThorsAnvil
{
    namespace FileSystem
    {
        using StrIndex = std::uint64_t;
        namespace ColumnFormat
        {
    /*
     * This header defines two main types.
     * FileBase and FileMembers
     *
     * The FileMembers class holds a tuple of all subfiles and mainly deals with distributing any call on the class to all submembers.
     * Just to prove all rules have exceptions, the FileMembers class also holds a state member that is the binary or of all the other
     * subfiles state flags. This makes checking the overall state of the object simler.
     *
     * The FileBase class acts as the logical File object.
     * It contains any buisness logic associated with a file (including its primary name).
     *
     * The user variants of FileBase are:
     *      IFile:      Input File
     *      OFile:      Output File
     *      File:       Can be either in or out or in/out just need to be specific in the open flag.
     */

    // Need to forward declare FileMembers as it is used in the Implementation details section.
    template<typename S, typename T>
    class FileMembers;

    namespace Impl
    {
        /*
         * This section contains some utility class to help in building types (there is no code here)
         * NamingConvention:
         *      XXXSelector:    A class partial specialization that helps select a type for XXX based on input type T.
         *      XXXBuilder:     A class partial specialization that builds a tuple type XXX based on input type Arg...
         *
         * Normally Selector and builder are used together to build a tuple of types.
         *
         * GetPointerMember:    Given a pointer to a member (Type). Returns the value being pointed at.
         * FileType:            Given a type T The type of file we will use to store it.
         *                      A: If T is a POD type this is type S (which will be one std::ifstream, std::ofstream, std::fstream)
         *                      B: If T is a std::string this type is a struct with S being used to hold data and std::fstream used to hold an index into the strings.
         *                         This is so given a position we can quickly seek to a position in the file where the string is held.
         *                      C: If T is an object mapped by ThorsAnvil_MakeTrait (see ThorsSerializer) then File<S, T>.
         *                         Thus we map a structure to multiple files one file for each members. If a member is another structure
         *                         this becomes a subdirectory with each of its memebrs mapped to a file in this subdirectory.
         * TupleFileType:       Given a class T; This becomes a std::tuple<FileType...> one member of the tuple for each member of the class.
         *
         * PreOpenState:        When opening a file we do a pre-scan to decide if any file will fail to open.
         *                      We track the state of how we are doing with this type so we can tidy up if we decide the open will fail.
         * OpenState:           Given a type T the type is used to store state for an attempted opening.
         *                      A: If T is POD or std::string then PreOpenState
         *                      B: If T is an object mapped by ThorsAnvil_MakeTrait (see ThorsSerializer) then std::tuple<OpenState...>
         * OpenStateTuple:      Given a class T; this becomes a std::tuple<OpenState...> one member of the tuple for each member of the class.
         * OpenMemberTuple:     A Utility to help.
         */

        // Get the type being pointed at by a pointer to member variable.
        template<typename P>
        struct GetPointerMember;

        template<typename R, typename T>
        struct GetPointerMember<std::pair<char const*, R T::*>>
        {
            using ReturnType = R;
        };

        template<typename P>
        using GetPointerMemberType = typename GetPointerMember<P>::ReturnType;

        /*
         * FileTypeSelector:    Use template specialization to define the stream class used.
         *                      For basic objects this is `std::fstream`
         *                      For Json::Map types use a FileMembers<S, T> types as this will recursively contain
         *                      File<M> or `std::fstream` types.
         */
        template<typename S, typename T, ThorsAnvil::Serialize::TraitType type = ThorsAnvil::Serialize::Traits<T>::type>
        struct FileTypeSelector;

        template<typename S, typename T>
        struct FileTypeSelector<S, T, ThorsAnvil::Serialize::TraitType::Value>
        {
            using FileType  = S;
        };

        template<typename S>
        struct FileTypeSelector<S, std::string, ThorsAnvil::Serialize::TraitType::Value>
        {
            using FileType  = struct FileTypeStruct
            {
                S               data;
                std::fstream    index;
            };
        };

        template<typename S, typename T>
        struct FileTypeSelector<S, T, ThorsAnvil::Serialize::TraitType::Map>
        {
            using FileType  = FileMembers<S, T>;
        };

        template<typename S, typename T>
        using FileType = typename FileTypeSelector<S, T>::FileType;

        /*
         * FileTupleBuilder:    Iterate over a tuple to get the stream types.
         */
        template<typename S, typename T, typename TUP = typename ThorsAnvil::Serialize::Traits<T>::Members>
        struct TupleFileTypeBuilder;

        template<typename S, typename T, typename... Args>
        struct TupleFileTypeBuilder<S, T, std::tuple<Args...>>
        {
            using TupleFileType = std::tuple<FileType<S, GetPointerMemberType<Args>>...>;
        };

        template<typename S, typename T>
        using TupleFileType = typename TupleFileTypeBuilder<S, T>::TupleFileType;

        /*
         * OpenStateSelector:   Select if we use PreOpenState (for std::fstream) or a struct (for FileMembers)
         */
        enum PreOpenState {NoAction, NoDir, DirExists};

        template<typename T, ThorsAnvil::Serialize::TraitType type = ThorsAnvil::Serialize::Traits<T>::type>
        struct OpenStateSelector;

        /*
         * OpenStateBuilder:   Build a tuple of (OpenStateSelector) for the underlying stream types.
         */
        template<typename T>
        struct OpenStateTupleBuilder;

        template<typename... Args>
        struct OpenStateTupleBuilder<std::tuple<Args...>>
        {
            using OpenStateTuple = std::tuple<typename OpenStateSelector<GetPointerMemberType<Args>>::OpenState...>;
        };

        template<typename T>
        using OpenStateTuple = typename OpenStateTupleBuilder<T>::OpenStateTuple;

        template<typename T>
        struct OpenStateSelector<T, ThorsAnvil::Serialize::TraitType::Value>
        {
            using OpenState = PreOpenState;
        };

        template<typename T>
        struct OpenStateSelector<T, ThorsAnvil::Serialize::TraitType::Map>
        {
            struct OpenState
            {
                using OpenMemberTuple = OpenStateTuple<typename ThorsAnvil::Serialize::Traits<T>::Members>;
                PreOpenState        base;
                OpenMemberTuple     members;
            };
        };

        /*
         * The types used after we have built it from the above
         */
        template<typename T>
        using OpenState      = typename OpenStateSelector<T>::OpenState;
        template<typename T>
        using OpenMemberTuple = typename OpenState<T>::OpenMemberTuple;

        // Forward declaration of FileAccessObjectType See file.tpp for details.
        template<typename F, typename T, ThorsAnvil::Serialize::TraitType type = ThorsAnvil::Serialize::Traits<T>::type>
        struct FileAccessObjectType;
    }

    using streampos = unsigned long;
    using streamoff = signed long;
    using seekdir   = std::ios_base::seekdir;

    template<typename S, typename T>
    class FileMembers
    {
        using Traits    = ThorsAnvil::Serialize::Traits<T>;
        using Members   = typename Traits::Members;
        using Index     = std::make_index_sequence<std::tuple_size<Members>::value>;

        using FileTuple = Impl::TupleFileType<S, T>;

        FileTuple       fileTuple;
        iostate         state;

        public:
             FileMembers();

            Impl::OpenState<T> doOpenTry(bool& ok, std::string const& path, openmode mode);
            void doOpenFin(bool ok, std::string const& path, openmode mode, Impl::OpenState<T> const& state);
            void close()                            {doCloseMembers(Index{});}
            void read(T& data)                      {readMembers(data, Index{});}
            void write(T const& data)               {writeMembers(data, Index{});}
            void setstate(iostate extraState)       {setstateLocalOnly(extraState); setstateMembers(extraState, Index{});}
            void clear(iostate newState = goodbit)  {clearLocalOnly(newState);      clearMembers(newState, Index{});}
            void seekg(streampos pos)               {seekgMembers(pos, Index{});}
            void seekp(streampos pos)               {seekpMembers(pos, Index{});}

            // https://en.cppreference.com/w/cpp/io/ios_base/iostate
            bool good()                             const   {return !(state & (eofbit | badbit | failbit));}
            bool eof()                              const   {return state & eofbit;}
            bool bad()                              const   {return state & badbit;}
            bool fail()                             const   {return state & (failbit | badbit);}
            operator bool()                         const   {return !fail();}
            bool operator!()                        const   {return !static_cast<bool>(*this);}
            iostate rdstate()                       const   {return state;}
        private:
            template<std::size_t... I>
            Impl::OpenMemberTuple<T> doOpenTryMembers(bool& ok, std::string const& path, openmode mode, std::index_sequence<I...>);
            template<std::size_t... I>
            void doOpenFinMembers(bool ok, std::string const& path, openmode mode, Impl::OpenMemberTuple<T> const& state, std::index_sequence<I...>);

            template<std::size_t... I>
            void doCloseMembers(std::index_sequence<I...>);

            template<std::size_t... I>
            void readMembers(T& data, std::index_sequence<I...>);

            template<std::size_t... I>
            void writeMembers(T const& data, std::index_sequence<I...>);

            template<std::size_t... I>
            void setstateMembers(iostate extraState, std::index_sequence<I...>);

            template<std::size_t... I>
            void clearMembers(iostate newState, std::index_sequence<I...>);

            template<std::size_t... I>
            void seekgMembers(streampos pos, std::index_sequence<I...>);

            template<std::size_t... I>
            void seekpMembers(streampos pos, std::index_sequence<I...>);

            template<std::size_t I>
            std::string getMemberFilePath(std::string const& path);

        protected:
            void setstateLocalOnly(iostate extraState)      {state |= extraState;}
            void clearLocalOnly(iostate newState)           {state = newState;}
    };

    template<typename S, typename T>
    class FileBase: public FileMembers<S, T>
    {
        bool            fileOpened;
        std::string     baseFileName;
        std::fstream    index;
        streampos       getPos;
        streampos       putPos;

        public:
            FileBase(std::string fileName = "", openmode mode = 0);

            void open(std::string fileName, openmode mode = 0);
            void close();

            void read(T& data);
            void write(T const& data);

            streampos   tellg() {return getPos;}
            streampos   tellp() {return putPos;}
            void        seekg(streampos pos);
            void        seekp(streampos pos);
            void        seekg(streamoff off, seekdir dir);
            void        seekp(streamoff off, seekdir dir);

            friend FileBase& operator>>(FileBase& file, T& data)
            {
                file.read(data);
                return file;
            }
            friend FileBase& operator<<(FileBase& file, T const& data)
            {
                file.write(data);
                return file;
            }
        private:
            void open(openmode mode);
    };

    template<typename T>
    class IFile: public FileBase<std::ifstream, T>
    {
        public:
            IFile(std::string fileName = "", openmode mode = std::ios::in)
                : FileBase<std::ifstream, T>(std::forward<std::string>(fileName), mode)
            {}
            void open(std::string fileName, openmode mode = std::ios::in)
            {
                return FileBase<std::ifstream, T>::open(std::forward<std::string>(fileName), mode);
            }
    };
    template<typename T>
    class OFile: public FileBase<std::ofstream, T>
    {
        public:
            OFile(std::string fileName = "", openmode mode = std::ios::out)
                : FileBase<std::ofstream, T>(std::forward<std::string>(fileName), mode)
            {}
            void open(std::string fileName, openmode mode = std::ios::out)
            {
                return FileBase<std::ofstream, T>::open(std::forward<std::string>(fileName), mode);
            }
    };
    template<typename T>
    class File: public FileBase<std::fstream, T>
    {
        public:
            File(std::string fileName = "", openmode mode = std::ios::in | std::ios::out)
                : FileBase<std::fstream, T>(std::forward<std::string>(fileName), mode)
            {}
            void open(std::string fileName, openmode mode = std::ios::in | std::ios::out)
            {
                return FileBase<std::fstream, T>::open(std::forward<std::string>(fileName), mode);
            }
    };

        }
    }
}

#include "file.tpp"

#endif
