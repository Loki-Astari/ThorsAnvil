#ifndef THORSANVIL_SERIALIZE_JSON_PARSER_H
#define THORSANVIL_SERIALIZE_JSON_PARSER_H
/*
 * JsonParser<T>
 *      This is used in conjunction with JsonPrinter<T>
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

#include "SerializeConfig.h"
#include "Serialize.h"
#include "JsonManualLexer.h"
#include <istream>
#include <string>
#include <vector>

namespace ThorsAnvil
{
    namespace Serialize
    {

class JsonParser: public ParserInterface
{
    enum State          {Error, Init, OpenM, Key, Colon, ValueM, CommaM, CloseM, OpenA, ValueA, CommaA, CloseA, ValueD, Done};

    JsonManualLexer     lexer;
    std::vector<State>  parrentState;
    State               currentEnd;
    State               currentState;
    bool                started;

    std::string getString();
    std::string getRawString();

    template<typename T>
    T scan();
    public:
        JsonParser(std::istream& stream, ParserConfig config = ParserConfig{});
        virtual FormatType formatType()                         override {return FormatType::Json;}
        virtual ParserToken getNextToken()                      override;
        virtual std::string getKey()                            override;

        virtual void    ignoreDataValue()                       override;

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

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "JsonParser.source"
#endif

#endif
