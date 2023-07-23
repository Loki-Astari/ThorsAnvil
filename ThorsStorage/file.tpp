#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_FILE_TPP
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_FILE_TPP

#include "file.h"

namespace ThorsAnvil
{
    namespace FileSystem
    {
        namespace ColumnFormat
        {

    namespace Impl
    {
        /*
         * FileAccessObjectType:        Given a type T knows how to access the underlying File for the type.
         *                              Note the file type will be defined in file.h by Impl::FileType
         *
         *                              Note we use the template type F to represent the type as FileType is specialized
         *                              by S (the stream) which could be (std::ifstream, std::ofstream, std::stream).
         *
         * But there are three basic versions:
         *                      Given a type T The type of file we will use to store it.
         *                      A: If T is a POD type this is type S (which will be one std::ifstream, std::ofstream, std::fstream)
         *                              For most operations we simply pass on the call,
         *                      B: If T is a std::string this type is a struct with S being used to hold data and std::fstream used to hold an index into the strings.
         *                              For most operations we simply pass on the call.
         *                              For writes we add an additional write for the index to the start of the next line.
         *                              seekp() and seekg() read the index files before seeking in the data file.
         *                      C: If T is an object mapped by ThorsAnvil_MakeTrait (see ThorsSerializer) then File<S, T>.
         *                              For most operations we simply pass on the call,
         */

        // Default versions handles case C: the File type is File<S, T>
        template<typename F, typename T, ThorsAnvil::Serialize::TraitType type>
        struct FileAccessObjectType
        {
            F&  file;
            FileAccessObjectType(F& file)
                : file(file)
            {}

            OpenState<T> openTry(bool& ok, std::string&& path, openmode mode)
            {
                return file.doOpenTry(ok, std::move(path), mode);
            }
            void openFinalize(bool ok, std::string&& path, openmode mode, OpenState<T> const& state)
            {
                file.doOpenFin(ok, std::move(path), mode, state);
            }
            void close()                                {file.close();}
            void read(T& obj)                           {file.read(obj);}
            void write(T const& obj) const              {file.write(obj);}
            void setstate(iostate extraState)           {file.setstate(extraState);}
            void clear(iostate newState)                {file.clear(newState);}
            iostate rdstate() const                     {return file.rdstate();}
            void seekg(streampos pos)                   {file.seekg(pos);}
            void seekp(streampos pos)                   {file.seekp(pos);}
        };

        // This specialization for Value types handles all POD types and F is a standrard library strean
        template<typename F, typename T>
        struct FileAccessObjectType<F, T, ThorsAnvil::Serialize::TraitType::Value>
        {
            F&  file;
            FileAccessObjectType(F& file)
                : file(file)
            {}

            PreOpenState openTry(bool& ok, std::string const& path, openmode mode)
            {
                ok = ok && FileSystem::isFileOpenable(path, mode);
                return NoAction;
            }
            void openFinalize(bool ok, std::string const& path, openmode mode, PreOpenState const&)
            {
                if (ok)
                {
                    file.open(path.c_str(), mode | std::ios_base::binary);
                }
            }
            void close()                                {file.close();}
            void read(T& obj)                           {file.read(reinterpret_cast<char*>(&obj), sizeof(T));}
            void write(T const& obj) const              {file.write(reinterpret_cast<char const*>(&obj), sizeof(T));}
            void setstate(iostate extraState)           {file.setstate(extraState);}
            void clear(iostate newState)                {file.clear(newState);}
            iostate rdstate() const                     {return file.rdstate();}
            void seekg(streampos pos)                   {file.seekg(pos * sizeof(T));}
            void seekp(streampos pos)                   {file.seekp(pos * sizeof(T));}
        };

        // This specialization for std::string keeps track of the data and an index into the data.
        template<typename F>
        struct FileAccessObjectType<F, std::string, ThorsAnvil::Serialize::TraitType::Value>
        {
            F&  file;
            FileAccessObjectType(F& file)
                : file(file)
            {}

            PreOpenState openTry(bool& ok, std::string const& path, openmode mode)
            {
                ok = ok && FileSystem::isFileOpenable(path, mode);
                return NoAction;
            }
            void openFinalize(bool ok, std::string const& path, openmode mode, PreOpenState const&)
            {
                if (ok)
                {
                    file.data.open(path, mode | std::ios_base::binary);
                    {
                        std::ofstream touch(path + ".index", std::ios::app);
                    }
                    file.index.open(path + ".index", mode | std::ios_base::in | std::ios_base::out | std::ios_base::binary);
                }
            }
            void close()
            {
                file.data.close();
                file.index.close();
            }
            void read(std::string& obj)
            {
                std::getline(file.data, obj);
                std::transform(std::begin(obj), std::end(obj), std::begin(obj), [](char x){return x == '\0' ? '\n' : x;});
            }
            void write(std::string const& obj)
            {
                std::string::const_iterator start   = std::begin(obj);
                std::size_t                 used    = 0;

                for (std::size_t size = obj.find('\n'); size != std::string::npos; size = obj.find('\n', size + 1))
                {
                    size = (size == std::string::npos) ? (std::end(obj) - start) : size;
                    std::size_t len = (size - used);
                    file.data << std::string_view(&*start, size - used) << '\0';

                    start += (len + 1);
                    used  += (len + 1);
                }
                file.data << std::string_view(&*start) << "\n";
                StrIndex index = static_cast<StrIndex>(file.data.tellp());
                file.index.write(reinterpret_cast<char*>(&index), sizeof(index));
            }
            void setstate(iostate extraState)           {file.data.setstate(extraState);file.index.setstate(extraState);}
            void clear(iostate newState)                {file.data.clear(newState);file.index.clear(newState);}
            iostate rdstate() const                     {return file.data.rdstate() | file.index.rdstate();}
            void seekg(streampos pos)
            {
                if (pos == 0)
                {
                    file.index.seekg(0);
                    file.data.seekg(0);
                }
                else
                {
                    file.index.seekg(pos * sizeof(StrIndex) - sizeof(StrIndex));
                    StrIndex index;
                    file.index.read(reinterpret_cast<char*>(&index), sizeof(index));
                    file.data.seekg(static_cast<std::streampos>(index));
                }
            }
            void seekp(streampos pos)
            {
                if (pos == 0)
                {
                    file.index.seekp(0);
                    file.data.seekp(0);
                }
                else
                {
                    file.index.seekg(pos * sizeof(StrIndex) - sizeof(StrIndex));
                    StrIndex index;
                    file.index.read(reinterpret_cast<char*>(&index), sizeof(index));
                    file.index.seekp(pos * sizeof(StrIndex) - sizeof(StrIndex));
                    file.data.seekp(static_cast<std::streampos>(index));
                }
            }
        };

        template<typename S, typename T, std::size_t I>
        struct FileAccessObjectSelector
        {
            using Traits            = ThorsAnvil::Serialize::Traits<T>;
            using Members           = typename Traits::Members;
            using FileTuple         = TupleFileType<S, T>;

            using FileIndex         = std::tuple_element_t<I, FileTuple>;
            using PointerTypeIndex  = std::tuple_element_t<I, Members>;
            using DstIndex          = GetPointerMemberType<PointerTypeIndex>;

            using FileAccessObject  = FileAccessObjectType<FileIndex, DstIndex>;
        };
        // Given an S, T and an index I.
        template<typename S, typename T, std::size_t I>
        using FileAccessObject      = typename FileAccessObjectSelector<S, T, I>::FileAccessObject;

    }

    // ==== FileMembers ====

    template<typename S, typename T>
    inline FileMembers<S, T>::FileMembers()
        : state(failbit)
    {}

    // ---- Open ----

    // void FileBase<S, T>::open for a description of the open processes.
    template<typename S, typename T>
    inline Impl::OpenState<T> FileMembers<S, T>::doOpenTry(bool& ok, std::string const& path, openmode mode)
    {
        Impl::OpenState<T>  result;
        if (!ok)
        {
            result.base = Impl::NoAction;
            return result;
        }

        FileSystem::DirResult createDir = FileSystem::makeDirectory(path, 0'777);

        if (createDir == FileSystem::DirFailedToCreate)
        {
            ok          = false;
            result.base = Impl::NoAction;
            return result;
        }

        result.base = createDir == FileSystem::DirAlreadyExists ? Impl::DirExists : Impl::NoDir;
        result.members = doOpenTryMembers(ok, path, mode, Index{});
        return result;
    }

    template<typename S, typename T>
    template<std::size_t... I>
    inline Impl::OpenMemberTuple<T> FileMembers<S, T>::doOpenTryMembers(bool& ok, std::string const& path, openmode mode, std::index_sequence<I...>)
    {
        Impl::OpenMemberTuple<T> result = std::make_tuple([this, &ok, &path, mode]()
        {
            Impl::FileAccessObject<S, T, I>  fileAccess(std::get<I>(fileTuple));

            auto result = fileAccess.openTry(ok, getMemberFilePath<I>(path), mode);
            setstateLocalOnly(fileAccess.rdstate());
            return result;
        }()...);

        return result;
    }

    template<typename S, typename T>
    inline void FileMembers<S, T>::doOpenFin(bool ok, std::string const& path, openmode mode, Impl::OpenState<T> const& state)
    {
        if (state.base == Impl::NoAction)
        {
            return;
        }

        doOpenFinMembers(ok, path,  mode, state.members, Index{});

        if (!ok && state.base == Impl::NoDir)
        {
            FileSystem::removeFileOrDirectory(path);
            // We should probably log something if we fail to remove the directory.
            // I don't think an exception is appropriate at this point we have already failed
            // to create the file if this is the issue then we don't want to create in appropriate errors and a few
            // extra directories in the file system is not going to hurt
        }
    }

    template<typename S, typename T>
    template<std::size_t... I>
    inline void FileMembers<S, T>::doOpenFinMembers(bool ok, std::string const& path, openmode mode, Impl::OpenMemberTuple<T> const& state, std::index_sequence<I...>)
    {
        ([this, ok, &path, mode, &state]()
        {
            Impl::FileAccessObject<S, T, I>  fileAccess(std::get<I>(fileTuple));

            fileAccess.openFinalize(ok, getMemberFilePath<I>(path), mode, std::get<I>(state));
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    // ---- Close ----

    template<typename S, typename T>
    template<std::size_t... I>
    inline void FileMembers<S, T>::doCloseMembers(std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this]()
        {
            Impl::FileAccessObject<S, T, I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.close();
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    // ---- Read/Write ----

    template<typename S, typename T>
    template<std::size_t... I>
    inline void FileMembers<S, T>::readMembers(T& data, std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this, &data]()
        {
            Impl::FileAccessObject<S, T, I>  fileAccess(std::get<I>(fileTuple));
            auto& members       = Traits::getMembers();
            auto& pointer       = std::get<I>(members).second;

            fileAccess.read(data.*pointer);
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    template<typename S, typename T>
    template<std::size_t... I>
    inline void FileMembers<S, T>::writeMembers(T const& data, std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this, &data]()
        {
            Impl::FileAccessObject<S, T, I>  fileAccess(std::get<I>(fileTuple));
            auto& members       = Traits::getMembers();
            auto& pointer       = std::get<I>(members).second;

            fileAccess.write(data.*pointer);
            setstateLocalOnly(fileAccess.rdstate());
        }(), ...);
    }

    // ---- Clear State Bits ----

    template<typename S, typename T>
    template<std::size_t... I>
    inline void FileMembers<S, T>::clearMembers(iostate newState, std::index_sequence<I...>)
    {
        // Using fold expression and lambda.
        ([this, newState]()
        {
            Impl::FileAccessObject<S, T, I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.clear(newState);
        }(), ...);
    }

    // ---- Set State Bits ----

    template<typename S, typename T>
    template<std::size_t... I>
    inline void FileMembers<S, T>::setstateMembers(iostate extraState, std::index_sequence<I...>)
    {
        ([this, extraState]()
        {
            Impl::FileAccessObject<S, T, I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.setstate(extraState);
        }(), ...);
    }

    // ---- seek ----
    template<typename S, typename T>
    template<std::size_t... I>
    inline void FileMembers<S, T>::seekgMembers(streampos pos, std::index_sequence<I...>)
    {
        ([this, pos]()
        {
            Impl::FileAccessObject<S, T, I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.seekg(pos);
        }(), ...);
    }

    template<typename S, typename T>
    template<std::size_t... I>
    inline void FileMembers<S, T>::seekpMembers(streampos pos, std::index_sequence<I...>)
    {
        ([this, pos]()
        {
            Impl::FileAccessObject<S, T, I>  fileAccess(std::get<I>(fileTuple));
            fileAccess.seekp(pos);
        }(), ...);
    }

    // ---- Get Index Element Path Name from base ----

    template<typename S, typename T>
    template<std::size_t I>
    inline std::string FileMembers<S, T>::getMemberFilePath(std::string const& path)
    {
        std::string filePath = path;
        filePath += "/";
        filePath += std::get<I>(Traits::getMembers()).first;

        return filePath;
    }

    // ===== FileBase =========

    template<typename S, typename T>
    inline FileBase<S, T>::FileBase(std::string fileName, openmode mode)
        : fileOpened(false)
        , baseFileName(std::move(fileName))
        , getPos(0)
        , putPos(0)
    {
        open(mode);
    }

    // ---- Open ----
    // Open is complex:
    //  Only the first function here is public.
    //  The second is the main entry point called by the public open and the constructor.
    //  It performs the open in two two stages:
    //      Stage 1:    doOpenTry:
    //                      Create Directory if they don't exist.
    //                      Check if required files can be opened in the required mode in a directory.
    //      Stage 2:    doOpenFin:
    //                      If all files can be created then create all files.
    //                      If we can not create all the files then remove the directories we created in stage 1.

    template<typename S, typename T>
    inline void FileBase<S, T>::open(std::string fileName, openmode mode)
    {
        if (fileOpened)
        {
            FileMembers<S, T>::setstate(failbit);
            return;
        }
        baseFileName = std::move(fileName);
        open(mode);
    }

    template<typename S, typename T>
    inline void FileBase<S, T>::open(openmode mode)
    {
        if (baseFileName == "")
        {
            return;
        }
        fileOpened = true;
        FileMembers<S, T>::clear();

        Impl::OpenState<T> state = FileMembers<S, T>::doOpenTry(fileOpened, baseFileName, mode);
        FileMembers<S, T>::doOpenFin(fileOpened, baseFileName, mode, state);

        if (!fileOpened)
        {
            FileMembers<S, T>::setstate(failbit);
        }
        else
        {
            index.open(baseFileName + "/$index", mode);
            getPos  = index.tellg();
            putPos  = index.tellp();
        }
    }

    // ---- close ----

    template<typename S, typename T>
    inline void FileBase<S, T>::close()
    {
        if (!fileOpened)
        {
            return;
        }
        FileMembers<S, T>::close();
        FileBase<S, T>::setstateLocalOnly(failbit);
        fileOpened = false;
    }

    // ---- read/write ----

    template<typename S, typename T>
    inline void FileBase<S, T>::read(T& data)
    {
        if (!FileMembers<S, T>::good())
        {
            return;
        }
        FileMembers<S, T>::read(data);
        char mark;
        index.read(&mark, 1);
        ++getPos;
    }

    template<typename S, typename T>
    inline void FileBase<S, T>::write(T const& data)
    {
        if (!FileMembers<S, T>::good())
        {
            return;
        }
        FileMembers<S, T>::write(data);
        char mark = 'A';
        index.write(&mark, 1);
        ++putPos;
    }

    // ---- tell/seek ----
    template<typename S, typename T>
    inline void FileBase<S, T>::seekg(streampos pos)
    {
        index.seekg(pos);
        FileMembers<S, T>::seekg(pos);
        getPos  = pos;
    }

    template<typename S, typename T>
    inline void FileBase<S, T>::seekp(streampos pos)
    {
        index.seekp(pos);
        FileMembers<S, T>::seekp(pos);
        putPos = pos;
    }
    template<typename S, typename T>
    inline void FileBase<S, T>::seekg(streamoff off, seekdir dir)
    {
        index.seekg(off, dir);
        streampos pos = index.tellg();
        seekg(pos);
    }
    template<typename S, typename T>
    inline void FileBase<S, T>::seekp(streamoff off, seekdir dir)
    {
        index.seekp(off, dir);
        streampos pos = index.tellp();
        seekp(pos);
    }

        }
    }
}

#endif
