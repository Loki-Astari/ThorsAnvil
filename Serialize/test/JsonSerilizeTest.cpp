
#include "gtest/gtest.h"
#include "json.h"

#include <map>

using ThorsAnvil::Serialize::jsonImport;
using ThorsAnvil::Serialize::jsonExport;


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

void ValidateSerializedStrings(std::string lhs, std::string rhs)
{
    lhs.erase(std::remove(lhs.begin(), lhs.end(), ' '), lhs.end());
    rhs.erase(std::remove(rhs.begin(), rhs.end(), ' '), rhs.end());

    ASSERT_EQ(lhs, rhs);
}


TEST(JsonSerialize, JsonMap)
{
    std::string input   = "{\"action\": 15}";
    std::string result  = testAction<std::map<std::string, int> >(input);
    ValidateSerializedStrings(input, result);
}

TEST(JsonSerialize, JsonMapWithMap)
{
    std::string input   = "{\"action\": {\"data\": 15, \"location\": 12}}";
    std::string result  = testAction<std::map<std::string, std::map<std::string, int> > >(input);
    ValidateSerializedStrings(input, result);
}

TEST(JsonSerialize, JsonMapWithMapNotStringKey)
{
    std::string input   = "[ {\"first\": 1, \"second\": {\"data\": 23, \"location\": 345}}, {\"first\": 2, \"second\": { \"data\": 18, \"location\": 567}}]";
    std::string result  = testAction<std::map<int, std::map<std::string, int> > >(input);
    ValidateSerializedStrings(input, result);
}

TEST(JsonSerialize, JsonMapWithStringValues)
{
    std::string input  = "{\"action\": \"foo\", \"widget\": \"bar\"}";
    std::string result = testAction<std::map<std::string, std::string> >(input);
    ValidateSerializedStrings(input, result);
}

TEST(JsonSerialize, JsonArrayOfPOD)
{
    std::string input   = "[ 1, 2, 3, 4, 5, 6, 7]";
    std::string result  = testAction<std::vector<int> >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, JsonArrayOfArray)
{
    std::string input   = "[ [1, 2, 3, 4, 5, 6, 7], [1,2,3,4], [90,100,200,300]]";
    std::string result  = testAction<std::vector<std::vector<int> > >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, JsonArrayOfMap)
{
    std::string input   = "[ {\"alpha\": 123, \"beta\": 987, \"gama\": 200}, {\"alpha\": 45, \"beta\": 567, \"plop\": 56}]";
    std::string result  = testAction<std::vector<std::map<std::string, int> > >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, SetOfPOD)
{
    std::string input   = "[ 1, 2, 3, 4, 5, 6 ]";
    std::string result  = testAction<std::set<int> >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, SetOfMap)
{
    std::string input   = "[ {\"a\": 1, \"b\": 2, \"c\": 3}, {\"a\": 2, \"b\": 2, \"c\": 2}, {\"a\": 2, \"b\":2, \"c\": 3}]";
    std::string result  = testAction<std::set<std::map<std::string, int> > >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, DeQueOfPOD)
{
    std::string input   = "[ 1, 2, 3, 4, 5, 6, 7, 8, 9]";
    std::string result  = testAction<std::deque<int> >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, DeQueOfSet)
{
    std::string input   = "[ [1,2,3,4], [4,5,6,7], [12,23,34]]";
    std::string result  = testAction<std::deque<std::set<int> > >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, DeQueOfMap)
{
    std::string input   = "[ {\"a\":1, \"b\": 2}, {\"g\": 45, \"h\": 45}]";
    std::string result  = testAction<std::deque<std::map<std::string, int> > >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, DeQueOfVector)
{
    std::string input   = "[ [1,2,3,4], [4,5,6,7], [12,23,34]]";
    std::string result  = testAction<std::deque<std::vector<int> > >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, ListOfPOD)
{
    std::string input   = "[ 1,2,3,4, 4,5,6,7, 12,23,34]";
    std::string result  = testAction<std::list<int> >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, ListOfVector)
{
    std::string input   = "[ [1,2,3,4], [4,5,6,7], [12,23,34]]";
    std::string result  = testAction<std::list<std::vector<int> > >(input);
    ValidateSerializedStrings(input, result);
}
TEST(JsonSerialize, ListOfString)
{
    std::string input   = "[ \"String1\", \"String2\", \"String3\", \"String4\" ]";
    std::string result  = testAction<std::vector<std::string> >(input);
    ValidateSerializedStrings(input, result);
}

TEST(JsonSerialize, ListOfMap)
{
    std::string input   = "[ {\"a\":1, \"b\": 2}, {\"g\": 45, \"h\": 45}]";
    std::string result  = testAction<std::list<std::map<std::string, int> > >(input);
    ValidateSerializedStrings(input, result);
}


class EmptyJsonClass
{};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<EmptyJsonClass>
{
	typedef void                ParentType;
    static JsonSerializeType const  type    = Map;
    typedef void                  SerializeInfo;
};
}}}

TEST(JsonSerialize, EmptyJsonClass)
{
    std::string input   = "{}";
    std::string result  = testAction<EmptyJsonClass>(input);
    ValidateSerializedStrings(input, result);
}

class OneMemberJsonClass
{
public:
    int value;
    friend class ThorsAnvil::Serialize::Json::JsonSerializeTraits<OneMemberJsonClass>;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<OneMemberJsonClass>
{
    typedef OneMemberJsonClass  LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, OneMemberJsonClass)
{
    std::string input   = "{\"value\": 15}";
    std::string result  = testAction<OneMemberJsonClass>(input);
    ValidateSerializedStrings(input, result);
}

struct NestedJsonTest
{
    OneMemberJsonClass  nested;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<NestedJsonTest>
{
    typedef NestedJsonTest		LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(nested);
    typedef boost::mpl::vector<nested>   SerializeInfo;
};
}}}

TEST(JsonSerialize, NestedJsonTest)
{
    std::string input   = "{\"nested\": {\"value\": 105}}";
    std::string result  = testAction<NestedJsonTest>(input);
    ValidateSerializedStrings(input, result);
}

// #####

struct TestFloat
{
    float value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestFloat>
{
    typedef TestFloat			LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestFloat)
{
    std::string input   = "{\"value\": 1234.45}";
    std::string result  = testAction<TestFloat>(input);
    ValidateSerializedStrings(input, result);
}

// #####

struct TestDouble
{
    double value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestDouble>
{
    typedef TestDouble			LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestDouble)
{
    std::string input   = "{\"value\": 1234.45}";
    std::string result  = testAction<TestDouble>(input);
    ValidateSerializedStrings(input, result);
}


// #####

struct TestShort
{
    short value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestShort>
{
    typedef TestShort			LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestShort)
{
    std::string input   = "{\"value\": 23}";
    std::string result  = testAction<TestShort>(input);
    ValidateSerializedStrings(input, result);
}

// #####

struct TestInt
{
    int value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestInt>
{
    typedef TestInt				LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestInt)
{
    std::string input   = "{\"value\": 23}";
    std::string result  = testAction<TestInt>(input);
    ValidateSerializedStrings(input, result);
}

// #####

struct TestLong
{
    long value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestLong>
{
    typedef TestLong			LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestLong)
{
    std::string input   = "{\"value\": 23}";
    std::string result  = testAction<TestLong>(input);
    ValidateSerializedStrings(input, result);
}


// #####
#if 0
struct TestLongLong
{
    long long value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestLongLong>
{
    typedef TestLongLong		LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestLongLong)
{
    std::string input   = "{\"value\": 23}";
    std::string result  = testAction<TestLongLong>(input);
    ValidateSerializedStrings(input, result);
}
#endif

// #####

struct TestUnsignedShort
{
    unsigned short value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestUnsignedShort>
{
    typedef TestUnsignedShort   LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestUnsignedShort)
{
    std::string input   = "{\"value\": 23}";
    std::string result  = testAction<TestUnsignedShort>(input);
    ValidateSerializedStrings(input, result);
}

// #####

struct TestUnsignedInt
{
    unsigned int value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestUnsignedInt>
{
    typedef TestUnsignedInt		LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestUnsignedInt)
{
    std::string input   = "{\"value\": 23}";
    std::string result  = testAction<TestUnsignedInt>(input);
    ValidateSerializedStrings(input, result);
}

// #####

struct TestUnsignedLong
{
    unsigned long value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestUnsignedLong>
{
    typedef TestUnsignedLong    LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestUnsignedLong)
{
    std::string input   = "{\"value\": 23}";
    std::string result  = testAction<TestUnsignedLong>(input);
    ValidateSerializedStrings(input, result);
}


// #####
#if 0
struct TestUnsignedLongLong
{
    unsigned long long value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestUnsignedLongLong>
{
    typedef TestUnsignedLongLong    LocalType;
	typedef void					ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestUnsignedLongLong)
{
    std::string input   = "{\"value\": 23}";
    std::string result  = testAction<TestUnsignedLongLong>(input);
    ValidateSerializedStrings(input, result);
}
#endif


// #####

struct TestBool
{
    bool value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestBool>
{
    typedef TestBool			LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestBoolTrue)
{
    std::string input   = "{\"value\": true}";
    std::string result  = testAction<TestBool>(input);
    ValidateSerializedStrings(input, result);
}

TEST(JsonSerialize, TestBoolFalse)
{
    std::string input   = "{\"value\": false}";
    std::string result  = testAction<TestBool>(input);
    ValidateSerializedStrings(input, result);
}

// #####

struct TestString
{
    std::string value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestString>
{
    typedef TestString			LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestString)
{
    std::string input   = "{\"value\": \"Hi There this is a string\"}";
    std::string result  = testAction<TestString>(input);
    ValidateSerializedStrings(input, result);
}

// #####

struct TestPointer
{
    TestString*     value;
};

namespace ThorsAnvil { namespace Serialize { namespace Json {
template<>
struct JsonSerializeTraits<TestPointer>
{
    typedef TestPointer			LocalType;
	typedef void                ParentType;
    static  JsonSerializeType const       type    = Map;
    THORSANVIL_SERIALIZE_JsonAttribute(value);
    typedef boost::mpl::vector<value>   SerializeInfo;
};
}}}

TEST(JsonSerialize, TestPointerNULL)
{
    std::string input   = "{\"value\": null }";
    std::string result  = testAction<TestPointer>(input);
    ValidateSerializedStrings(input, result);
}

TEST(JsonSerialize, TestPointerNotNull)
{
    std::string input   = "{\"value\": {\"value\":\"plop\" }}";
    std::string result  = testAction<TestPointer>(input);
    ValidateSerializedStrings(input, result);
}


