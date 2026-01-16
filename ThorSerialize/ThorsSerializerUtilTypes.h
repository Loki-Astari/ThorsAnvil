#ifndef THORSANVIL_SERIALIZER_THORS_SERIALIZER_UTIL_TYPES_H
#define THORSANVIL_SERIALIZER_THORS_SERIALIZER_UTIL_TYPES_H

#include "SerializeConfig.h"

#include <cstdint>
#include <optional>
#include <functional>
#include <istream>
#include <cstddef>

namespace ThorsAnvil::Serialize
{

enum class FormatType   {Json, Yaml, Bson};
enum class ParseType    {Weak, Strict, Exact};
enum class ParserToken  {Error, DocStart, DocEnd, MapStart, MapEnd, ArrayStart, ArrayEnd, Key, Value};

template<typename T>
struct SharedInfo
{
    std::intmax_t       sharedPtrName;
    std::optional<T*>   data;
};

struct IgnoreCallBack
{
    using AppendFunc = std::function<void(char const*, std::size_t)>;
    using ReadFunc   = std::function<void(std::istream&, char*, std::size_t)>;
    using IgnoreFunc = std::function<void(std::istream&, std::size_t)>;
    AppendFunc  append  = [](char const*, std::size_t){};
    ReadFunc    read    = [](std::istream& s, char* d, std::size_t size){s.read(d, size);};
    IgnoreFunc  ignore  = [](std::istream& s, std::size_t size)         {s.ignore(size);};
};

}

#endif
