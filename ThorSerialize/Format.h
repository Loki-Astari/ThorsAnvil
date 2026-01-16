#ifndef THORSANVIL_SERIALIZE_FORMAT_H
#define THORSANVIL_SERIALIZE_FORMAT_H

#include "SerializeConfig.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/BsonThor.h"
#include "ThorSerialize/YamlThor.h"

#include <format>
#include <string>
#include <sstream>
#include <cstddef>

namespace TA
{
    template<typename T>
    struct Ser
    {
        T const& value;
        Ser(T const& value)
            : value(value)
        {}
        Ser(Ser const&)         = default;
        Ser(Ser&&)              = default;
    };
};

template<typename T>
struct /*vera-disable*/ std::formatter<TA::Ser<T>>
{
    enum Format {Json, Bson, Yaml};
    Format                              form = Json;
    ThorsAnvil::Serialize::OutputType   type = ThorsAnvil::Serialize::OutputType::Stream;
    std::size_t                         indentSize = 0;
    std::size_t                         blockIndent = 0;

    constexpr bool isDigit(char val)
    {
        return val >= '0' && val <= '9';
    }
    constexpr auto parse(std::format_parse_context& ctx)
    {
        // Format specifier
        //   {:<Type>?<Stream>?<TabSize>?[.<blockIndent>]}
        //
        // Type: Stream Type: Optional. Default Json
        //      J       => Json
        //      B       => Bson
        //      Y       => Yaml
        //  Stream format: (Does not apply to Bson) Optional. Default Stream.
        //      -       => Stream (no extra white space single line. Good for Web Interface or log files)
        //      *       => Pretty print.
        // TabSize: (Does not apply to Bson or Stream). Default 0:  0 is 4 space for JSON or 2 space for YAML
        //      [0-9]*  => A positive value that represents the number of spaces used to indent each level.
        // BlockIndent: (Does not apply to Bson or Stream). An indent applied before any printing.
        //      [0-9]*  => A positive value that represents the space to inject on the left of an object.
        //                 To separate this from the "TabSize" it must be preceded by a '.'

        // loop: Is the current parse point of the format between the {} characters in the format string.
        auto loop = ctx.begin();

        // Check for the output type (Json/Bson/Yaml) specifier.
        // This is an optional single letter. J | B | Y
        switch (*loop)
        {
            case 'J':   form = Json; ++loop;    break;
            case 'B':   form = Bson; ++loop;    break;
            case 'Y':   form = Yaml; ++loop;    break;
            default:
                // Anything else fall through and out to the next test.
                break;
        }
        // Check the stream output (Stream / Pretty print)
        // This is an optional single letter. - | *
        switch (*loop)
        {
            case '-':   type = ThorsAnvil::Serialize::OutputType::Stream;   ++loop; break;
            case '*':   type = ThorsAnvil::Serialize::OutputType::Config;   ++loop; break;
            default:
                // Anything else fall through and out to the next test.
                break;
        }
        // Extract the indent size.
        // The default is 0. So it not supplied it will not change.
        for (;isDigit(*loop); ++loop) {
            indentSize = indentSize * 10 + (*loop - '0');
        }
        // Extract the block indent size.
        // Must be proceeded by a full stop.
        if (*loop == '.') {
            ++loop;
            for (;isDigit(*loop); ++loop) {
                blockIndent = blockIndent * 10 + (*loop - '0');
            }
        }
        return loop;
    }

    auto format(const TA::Ser<T>& obj, auto& ctx) const
    {
        // Create a serialization config object.
        ThorsAnvil::Serialize::PrinterConfig config{type, indentSize, blockIndent};

        std::stringstream ser;
        switch (form)
        {
            case Json:  ser << ThorsAnvil::Serialize::jsonExporter(obj.value, config);break;
            case Bson:  ser << ThorsAnvil::Serialize::bsonExporter(obj.value, config);break;
            case Yaml:  ser << ThorsAnvil::Serialize::yamlExporter(obj.value, config);break;
        }

        std::string const& out = ser.str();
        return ctx.advance_to(std::copy(std::begin(out), std::end(out), ctx.out()));
    }
};

#endif
