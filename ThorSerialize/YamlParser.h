#ifndef THORS_ANIVL_SERIALIZE_YAML_PARSER_H
#define THORS_ANIVL_SERIALIZE_YAML_PARSER_H
/*
 * YamlParser<T>
 *      This is used in conjunction with YamlPrinter<T>
 *
 *      Together these provide an implementation of:
 *          the ParserInterface for type T
 *          and PrinterInterface for type T
 *
 *      These Interfaces are used by Serializer and DeSerializer (see Serialize.h)
 *
 *      It uses ThorsAnvil::Serialize::Traits<T> to know what objects to pull from the stream.
 *      For arrays order is important.
 *      For Objects the order of elements is not important. It looks up the key in the Traits<T>
 *      information to understand which member is being de-serialized but unspecified elements
 *      will not cause an error.
 */


#include "Serialize.h"
#include <yaml.h>
#include <istream>
#include <vector>
#include <functional>

namespace ThorsAnvil
{
    namespace Serialize
    {

class YamlParser: public ParserInterface
{
    enum class State {Open, Map, Array};
    std::vector<std::pair<State, int>>  state;
    yaml_parser_t       parser;
    yaml_event_t        event;
    bool                first;
    bool                error;

    std::string getString();
    void generateParsingException(std::function<bool ()> test, std::string const& msg);
    ParserToken parsingError();

    template<typename T>
    T getValue(char* buffer, char** end);

    template<typename T>
    T scan();
    public:
        YamlParser(std::istream& input, ParserConfig config = ParserConfig{});
        virtual ~YamlParser();

        virtual FormatType formatType()                         override {return FormatType::Yaml;}
        virtual ParserToken     getNextToken()                  override;
        virtual std::string     getKey()                        override    {return getString();}

        virtual void    getValue(short int& value)              override;
        virtual void    getValue(int& value)                    override;
        virtual void    getValue(long int& value)               override;
        virtual void    getValue(long long int& value)          override;

        virtual void    getValue(unsigned short int& value)     override;
        virtual void    getValue(unsigned int& value)           override;
        virtual void    getValue(unsigned long int& value)      override;
        virtual void    getValue(unsigned long long int& value) override;

        virtual void    getValue(float& value)                  override;
        virtual void    getValue(double& value)                 override;
        virtual void    getValue(long double& value)            override;

        virtual void    getValue(bool& value)                   override;

        virtual void    getValue(std::string& value)            override;

        virtual bool    isValueNull()                           override;

        virtual std::string getRawValue()                       override;
};

    }
}

#if defined(HEADER_ONLY) && HEADER_ONLY == 1
#include "YamlParser.source"
#endif

#endif
