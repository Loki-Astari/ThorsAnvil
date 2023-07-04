#ifndef THORSANVIL_IOUTIL_FORMAT_H
#define THORSANVIL_IOUTIL_FORMAT_H

#include "ThorsIOUtilConfig.h"
#include "printToStream.h"
#include "Formatter.h"
#include "Utility.h"

#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <functional>
#include <algorithm>

namespace ThorsAnvil
{
    namespace IOUtil
    {

using ThorsAnvil::Utility::buildStringFromParts;

// @class-internal
// This is the object returned by make_format() and make_cppformat()
// This is the object that is passed to the `std::ostream` via `operator<<`.
//
// Internally it keeps the original string and references to all the parameters that
// need to be serialized. When the object is constructed (at runtime) the conversion
// specifiers in the string are validated against the actual parameters to make sure
// the correctly match.
template<typename... Args>
class Format
{
    Strictness                      strict;
    std::string                     format;
    std::tuple<Args const&...>      arguments;
    std::vector<std::string>        prefixString;
    std::vector<Formatter>          formater;
    public:
        Format(Strictness strict, char const* fmt, Args const&... args)
            : strict(strict)
            , format(fmt)
            , arguments(args...)
        {
            std::size_t count       = sizeof...(args);
            std::size_t pos         = 0;
            Dynamic     dynamicSize = Dynamic::None;
            for (std::size_t loop = 0; loop < count; ++loop)
            {
                // Find the prefix string before the next % sign (this may be empty)
                std::pair<std::string, std::size_t> prefix = getNextPrefix(format, pos, [](std::size_t p){return p == std::string::npos;}, "not enough format");
                pos += prefix.second;
                prefixString.emplace_back(std::move(prefix.first));

                // Now that I have found the %
                // Convert the next part of the string into a Formatter object.
                formater.emplace_back(strict, format.data() + pos, dynamicSize);
                pos         += formater.back().size();

                // Take into account the Dynamic Width/Precesion prevent the format from being read.
                // Note we can have Width/Precision or Both.
                Dynamic     newDynamic  = formater.back().isDynamicSize();
                dynamicSize = (newDynamic == Dynamic::Precision && dynamicSize == Dynamic::Width) ? Dynamic::Both : newDynamic;
            }
            // After the last formatter check for a last fixed string (this may be empty)
            // But there better not be any more % signs as we don't have any parameters left for them.
            std::pair<std::string, std::size_t> prefix = getNextPrefix(format, pos, [](std::size_t p){return p != std::string::npos;}, "too many format");
            pos += prefix.second;
            prefixString.emplace_back(std::move(prefix.first));
        }
        // Using the operator<< is the same as calling print on the object.
        friend std::ostream& operator<<(std::ostream& s, Format const& format)
        {
            format.print(s);
            return s;
        }
        void print(std::ostream& s) const
        {
            doPrint(s, std::make_index_sequence<sizeof...(Args)>());
        }
    private:
        // This finds the next '%' taking into account that %% is a not a scan token.
        std::pair<std::string, std::size_t> getNextPrefix(std::string const&, std::size_t pos, std::function<bool(std::size_t)>&& test, char const* mes)
        {
            std::string prefix;
            std::size_t extra = 0;
            std::size_t nextFormatter = format.find('%', pos);
            while (nextFormatter != std::string::npos && format[nextFormatter + 1] == '%')
            {
                nextFormatter = format.find('%', nextFormatter + 2);
            }
            prefix += format.substr(pos, nextFormatter - pos);
            prefix.erase(std::remove_if(std::begin(prefix), std::end(prefix), [first = false](char val) mutable
            {
                if (val == '%')
                {
                    first = !first;
                    return !first;
                }
                return false;
            }), std::end(prefix));
            if (test(nextFormatter))
            {
                throw std::invalid_argument(buildStringFromParts("Invalid Format: ", mes, " specifiers for provided arguments"));
            }
            return {prefix, prefix.size() + extra};
        }
        // For each argument we pass
        // Pass a prefix string (might be empty) the formatter then the argument.
        template<std::size_t I>
        std::ostream& printValue(std::ostream& s) const
        {
            return s << prefixString[I] << formater[I] << std::get<I>(arguments);
        }

        // Print all the values in order by calling printValue() on each parameter.
        // Then print the final fixed string (might be empty)
        template<std::size_t... I>
        void doPrint(std::ostream& s, std::index_sequence<I...> const&) const
        {
            std::ostream* ignore[] = {nullptr, &printValue<I>(s)...};
            (void)ignore;
            s << prefixString.back();
        }

};

// @function-api
// Builds a `Format` object based on the types of the arguments.
// @param fmt       The format string.
// @param args      Template parameter pack. Each argument is matched to a conversion specifier.
// @return          A format object that keeps a const reference to each argument.
template<typename... Args>
Format<Args...> make_format(char const* fmt, Args const&... args)
{
    return Format<Args...>(Strictness::CTypeCompat, fmt, args...);
}

// @function-api
// Builds a `Format` object based on the types of the arguments.
//
// This version has slightly tighter restrictions on matching parameters to conversion specifiers.
// Like C++ the type system is much pickier than C.
// @param fmt       The format string.
// @param args      Template parameter pack. Each argument is matched to a conversion specifier.
// @return          A format object that keeps a const reference to each argument.
template<typename... Args>
Format<Args...> make_cppformat(char const* fmt, Args const&... args)
{
    return Format<Args...>(Strictness::CPPTypeStrict, fmt, args...);
}

    }
}

#endif
