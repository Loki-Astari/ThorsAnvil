#ifndef THORSANVIL_SERIALIZE_IMPORTER_H
#define THORSANVIL_SERIALIZE_IMPORTER_H
/*
 * The Importer simply wrap an object of type T so that when de-serialized
 * it creates an object of type DeSerializer and calls it appropriately.
 */

#include "SerializeConfig.h"
#include "Serialize.h"

namespace ThorsAnvil::Serialize
{

template<typename Format, typename T>
class Importer
{
    T&              value;
    ParserConfig    config;
    public:
        Importer(T& value, ParserConfig config = ParserConfig{})
            : value(value)
            , config(config)
        {}
        template<typename I>
        bool extract(I&& stream) const
        {
            typename Format::Parser     parser(stream, config);
            try
            {
                DeSerializer                deSerializer(parser);

                deSerializer.parse(value);

                if (config.validateNoTrailingData && parser.ok())
                {
                    // This reads the next non space character.
                    char next;
                    parser.readValue(next);

                    // If there is only space left on the stream.
                    // This will result in the stream going bad.
                    // This indicates that we have succeeded.
                    if (!parser.ok())
                    {
                        // A fail means we are good.
                        // And we should clear it.
                        parser.clear();
                    }
                    else
                    {
                        // If the stream is OK then there was junk on the stream.
                        // So we have effectively failed.
                        ThorsLogAndThrowError(std::runtime_error, "ThorsAnvil::Serializer::Importer", "extract", "Validating no trailing data fail");
                    }
                }
            }
            catch (DepricatedIssue const& e)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Importer", "operator>>", e.what());
                ThorsRethrowMessage("ThorsAnvil::Serialize::Importer", "operator>>", e.what());
                // This exception is thrown because you are using deprecated code
                // that was not designed to be used with the bsonExporter
                // This must be fixed. So we are forcing a re-throw becuase
                // the generated binary object is probably bad.
                throw;
            }
            catch (std::exception const& e)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Importer", "operator>>", e.what());
                parser.setFail();
                if (!config.catchExceptions)
                {
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Importer", "operator>>", e.what());
                    throw;
                }
            }
            catch (...)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Importer", "operator>>", "UNKNOWN");
                parser.setFail();
                if (!config.catchUnknownExceptions)
                {
                    // Don't set this to true if you are using boost-coroutines.
                    // As this may prevent the force_unwind exception propagating.
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Importer", "operator>>", "UNKNOWN");
                    throw;
                }
            }
            return parser.ok();
        }
        friend std::istream& operator>>(std::istream& stream, Importer const& data)
        {
            data.extract(stream);
            return stream;
        }
        friend bool operator>>(std::string const& stream, Importer const& data)
        {
            return data.extract(std::string_view(stream.c_str(), stream.size()));
        }
        friend bool operator>>(std::string_view const& stream, Importer const& data)
        {
            return data.extract(stream);
        }
};

template<typename Format, typename T>
Importer<Format, T> Import(T const& value, ParserConfig config = ParserConfig{})
{
    return Importer<Format, T>(value, config);
}


}

#endif
