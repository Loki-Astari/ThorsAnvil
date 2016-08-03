
#ifndef THORS_ANVIL_SERIALIZE_SERIALIZE_H
#define THORS_ANVIL_SERIALIZE_SERIALIZE_H

/*
 * This is the guts of the serialization code.
 *
 * It uses template meta programming to generate the appropriate code to
 * serialize a type T. Thus most of the code required is generated at compile
 * time (there is a small run time overhead for Json/Yaml and a smaller
 * overhead for Binary).
 *
 * The type ThorsAnvil::Serialize::Traits<T> is used to generate the code to
 * serialize an object of type T. If there is not specializations of the
 * Traits<T> for the type T the default version will generate a compile time
 * error (with hopefully a useful message).
 *
 * The code is Symetrical for Serialization and DeSerialization
 *
 *      Serialization                       DeSerialization                 Description
 *      =======================             =======================         ===========================
 *      PrinterInterface                    ParserInterface                 Implemented by a particular format.
 *                                                                          The Printer is passed a set of Open/Close/Values
 *                                                                          The Parser generates a set of Open/Close/Values
 *      Serializer                          DeSerializer                    The base class the calls the Printer/Parser
 *                                                                          Serializer will print an object by generating the
 *                                                                          appropriate calls to the Printer based on the shape
 *                                                                          of the object. Deserializer ask the parser for a
 *                                                                          sequence of events from the input stream and converts
 *                                                                          these into updates on the object.
 *      SerializeMember                     DeSerializeMember               Generated (at compile time) from the Traits<T> information
 *                                                                          for each member that needs to be printed/parsed
 */

#include "Traits.h"
#include <iostream>
#include <utility>

namespace ThorsAnvil
{
    namespace Serialize
    {

class ParserInterface
{
    public:
        enum class ParserToken {Error, DocStart, DocEnd, MapStart, MapEnd, ArrayStart, ArrayEnd, Key, Value};
        std::istream&   input;
        ParserToken     pushBack;

        ParserInterface(std::istream& input)
            : input(input)
            , pushBack(ParserToken::Error)
        {}
        virtual ~ParserInterface() {}
                ParserToken     getToken();
                void            pushBackToken(ParserToken token);
        virtual ParserToken     getNextToken()          = 0;
        virtual std::string     getKey()                = 0;

        virtual void    getValue(short int&)             = 0;
        virtual void    getValue(int&)                   = 0;
        virtual void    getValue(long int&)              = 0;
        virtual void    getValue(long long int&)         = 0;

        virtual void    getValue(unsigned short int&)    = 0;
        virtual void    getValue(unsigned int&)          = 0;
        virtual void    getValue(unsigned long int&)     = 0;
        virtual void    getValue(unsigned long long int&)= 0;

        virtual void    getValue(float&)                 = 0;
        virtual void    getValue(double&)                = 0;
        virtual void    getValue(long double&)           = 0;

        virtual void    getValue(bool&)                  = 0;

        virtual void    getValue(std::string&)           = 0;
};
class PrinterInterface
{
    public:
        enum class OutputType {Default, Stream, Config};
        // Default:     What ever the implementation likes.
        // Stream:      Compressed for over the wire protocol.
        // Config:      Human readable (potentially config file like)

        std::ostream&   output;
        OutputType      characteristics;

        PrinterInterface(std::ostream& output, OutputType characteristics = OutputType::Default)
            : output(output)
            , characteristics(characteristics)
        {}
        virtual ~PrinterInterface() {}
        virtual void openDoc()                          = 0;
        virtual void closeDoc()                         = 0;
        virtual void openMap()                          = 0;
        virtual void closeMap()                         = 0;
        virtual void openArray(std::size_t size)        = 0;
        virtual void closeArray()                       = 0;

        virtual void    addKey(std::string const& key)  = 0;

        virtual void    addValue(short int)             = 0;
        virtual void    addValue(int)                   = 0;
        virtual void    addValue(long int)              = 0;
        virtual void    addValue(long long int)         = 0;

        virtual void    addValue(unsigned short int)    = 0;
        virtual void    addValue(unsigned int)          = 0;
        virtual void    addValue(unsigned long int)     = 0;
        virtual void    addValue(unsigned long long int)= 0;

        virtual void    addValue(float)                 = 0;
        virtual void    addValue(double)                = 0;
        virtual void    addValue(long double)           = 0;

        virtual void    addValue(bool)                  = 0;

        virtual void    addValue(std::string const&)    = 0;

        void addValue(void*)        = delete;
        void addValue(void const*)  = delete;
};

template<typename T>
T scanValue(char const* buffer, char** end);
template<>  inline short              scanValue<short>(char const* buffer, char** end)              {return (short)std::strtol(buffer, end, 10);}
template<>  inline int                scanValue<int>(char const* buffer, char** end)                {return std::strtol(buffer, end, 10);}
template<>  inline long               scanValue<long>(char const* buffer, char** end)               {return std::strtol(buffer, end, 10);}
template<>  inline long long          scanValue<long long>(char const* buffer, char** end)          {return std::strtoll(buffer, end, 10);}

template<>  inline unsigned short     scanValue<unsigned short>(char const* buffer, char** end)     {return (short)std::strtoul(buffer, end, 10);}
template<>  inline unsigned int       scanValue<unsigned int>(char const* buffer, char** end)       {return std::strtoul(buffer, end, 10);}
template<>  inline unsigned long      scanValue<unsigned long>(char const* buffer, char** end)      {return std::strtoul(buffer, end, 10);}
template<>  inline unsigned long long scanValue<unsigned long long>(char const* buffer, char** end) {return std::strtoull(buffer, end, 10);}

template<>  inline float              scanValue<float>(char const* buffer, char** end)              {return std::strtof(buffer, end);}
template<>  inline double             scanValue<double>(char const* buffer, char** end)             {return std::strtod(buffer, end);}
template<>  inline long double        scanValue<long double>(char const* buffer, char** end)        {return std::strtold(buffer, end);}

class Serializer;
class DeSerializer;

template<TraitType type, typename T, typename I>
class ApplyActionToParent
{
    public:
        // Default do nothing.
        void printParentMembers(Serializer&, T const&)            {}
        void scanParentMember(DeSerializer&, I const&, T&)        {}
};

template<typename T, typename M, TraitType type = Traits<M>::type>
class DeSerializeMember
{
    using ParserToken = ParserInterface::ParserToken;
    public:
        DeSerializeMember(ParserInterface& parser, std::string const& key, T& object, std::pair<char const*, M T::*> const& memberInfo);
};

class DeSerializer
{
    using ParserToken = ParserInterface::ParserToken;
    ParserInterface&    parser;
    bool                root;

    template<typename T, typename Members, std::size_t... Seq>
    void scanEachMember(std::string const& key, T& object, Members const& member, std::index_sequence<Seq...> const&);

    template<typename T, typename... Members>
    void scanMembers(std::string const& key, T& object, std::tuple<Members...> const& members);

    template<typename T, typename I, typename Action>
    void scanMembers(I const& key, T& object, Action action);
    public:
        DeSerializer(ParserInterface& parser, bool root = true);
        ~DeSerializer();

        template<typename T>
        void parse(T& object);

        template<typename T, typename I>
        void scanObjectMembers(I const& key, T& object);
};

template<typename T, typename M, TraitType type = Traits<typename std::remove_cv<M>::type>::type>
class SerializeMember
{
    public:
        SerializeMember(PrinterInterface& printer, T const& object, std::pair<char const*, M T::*> const& memberInfo);
};

class Serializer
{
    PrinterInterface& printer;
    bool              root;

    template<typename T, typename Members, std::size_t... Seq>
    void printEachMember(T const& object, Members const& member, std::index_sequence<Seq...> const&);

    template<typename T, typename... Members>
    void printMembers(T const& object, std::tuple<Members...> const& members);

    template<typename T, typename Action>
    void printMembers(T const& object, Action action);

    public:
        Serializer(PrinterInterface& printer, bool root = true);
        ~Serializer();

        template<typename T>
        void print(T const& object);

        template<typename T>
        void printObjectMembers(T const& object);
};

/* ------------ ParserInterface ------------------------- */
inline ParserInterface::ParserToken ParserInterface::getToken()
{
    ParserToken result  = ParserToken::Error;

    if (pushBack != ParserToken::Error)
    {
        std::swap(pushBack, result);
    }
    else
    {
        result = this->getNextToken();
    }
    return result;
}
inline void ParserInterface::pushBackToken(ParserToken token)
{
    if (pushBack != ParserToken::Error)
    {
        throw std::runtime_error(
                    "ThorsAnvil::Serialize::ParserInterface::pushBackToken: "
                    "Push only allows for single push back. More than one token has been pushed back between reads."
              );
    }
    pushBack    = token;
}
/* ------------ DeSerializer ------------------------- */

inline DeSerializer::DeSerializer(ParserInterface& parser, bool root)
    : parser(parser)
    , root(root)
{
    if (root)
    {
        // Note:
        //  Note: all "root" elements are going to have a DocStart/DocEnd pair
        //  Just the outer set. So that is something that we will need to deal with
        //
        //  Note: We also need to take care of arrays at the top level
        //  We will get that in the next version
        if (parser.getToken() != ParserToken::DocStart)
        {   throw std::runtime_error("ThorsAnvil::Serialize::DeSerializer::DeSerializer: Invalid Doc Start");
        }
    }
}
inline DeSerializer::~DeSerializer()
{
#ifndef _MSC_VER	// Destructor shall not throw exception.
    if (root)
    {
        if (parser.getToken() != ParserToken::DocEnd)
        {   throw std::runtime_error("ThorsAnvil::Serialize::DeSerializer::~DeSerializer: Expected Doc End");
        }
    }
#endif
}

/* ------------ Serializer ------------------------- */

inline Serializer::Serializer(PrinterInterface& printer, bool root)
    : printer(printer)
    , root(root)
{
    if (root)
    {
        printer.openDoc();
    }
}
inline Serializer::~Serializer()
{
    if (root)
    {
        printer.closeDoc();
    }
}

    }
}

#ifndef COVERAGE_TEST
#include "Serialize.tpp"
#endif

#endif
