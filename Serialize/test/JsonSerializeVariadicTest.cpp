
#include "gtest/gtest.h"
#include "json.h"

#include <map>

using ThorsAnvil::Serialize::jsonImport;
using ThorsAnvil::Serialize::jsonExport;

class Config
{
    int         valInt;
    double      valDouble;

    friend class ThorsAnvil::Serialize::Json::JsonSerializeTraits<Config>;
};

/* This test is designed to test the ability to serialize structs with a
 * single member using the variadic macro */
struct SingleMemberConfig {
    Config foo;
};

//#ifdef CLOUD_USE_CPLUSPLUS11
JsonSerializeTraits_MAKE(void, Config, valInt, valDouble)
JsonSerializeTraits_MAKE(void, SingleMemberConfig, foo)
//#endif
template<typename T>
std::string testAction(std::string const& expected)
{
    T                   object;

    std::stringstream   input(expected);
    input  >> jsonImport(object);

    std::stringstream   output;
    output << jsonExport(object);

    return output.str();
}

extern void ValidateSerializedStrings(std::string lhs, std::string rhs);


TEST(JsonSerializeVardac, Serialize)
{
    std::string input   = "{\"valInt\": 12, \"valDouble\":1234.45}";
    std::string result  = testAction<Config>(input);
    ValidateSerializedStrings(input, result);
}

TEST(JsonSerializeVariadic, SerializeSingleMember) {
    std::string input   = "{\"foo\":{\"valInt\": 12, \"valDouble\":1234.45}}";
    std::string result  = testAction<SingleMemberConfig>(input);
    ValidateSerializedStrings(input, result);
}
