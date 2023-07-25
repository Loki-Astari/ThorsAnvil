#ifndef THORSANVIL_DB_MONGO_MONGO_QUERY_H
#define THORSANVIL_DB_MONGO_MONGO_QUERY_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"
#include <iterator>

namespace ThorsAnvil::DB::Mongo
{

namespace Concept
{
template<typename T>
concept IsQuery = requires {typename T::Query; };
}

enum class Remove {All, One};

template<typename T, typename Hint = std::string>
struct Query
{
    using CType = ConstructorType<T>;
    Query(CType init, Remove r = Remove::All, std::optional<Collation> collation = {}, std::optional<Hint> hint = {})
        : q(std::move(init))
        , limit(r == Remove::One ? 1 : 0)
        , collation(collation)
        , hint(hint)
    {}
    T                               q;
    std::uint32_t                   limit;
    std::optional<Collation>        collation;
    std::optional<Hint>             hint;
};

namespace QueryOp
{

/*** Extractos ***/
template<typename T>
struct NormalExtractor { using CType = T;};
template<typename T>
struct ValueExtractor { using CType = typename T::value_type;};


enum class BsonType {   Double = 1,     String = 2,             Object = 3,         Array = 4,
                        BinaryData = 5, ObjectId = 7,           Boolean = 8,        Date = 9,
                        Null = 10,      RegularExpression = 11, JavaScript = 13,    Int32 = 16,
                        Timestamp = 17, Int64 = 18,             Decimal128 = 19,    MinKey = -1,
                        MaxKey = 127
                    };

/*
 * query-comparison
 * https://www.mongodb.com/docs/manual/reference/operator/query-comparison/
 *      Equal                       => Eq
 *      Not Equal                   => Ne
 *      Greater                     => Gt
 *      Greater or Equal            => Gte
 *      Less                        => Lt
 *      Less or Equal               => Lte
 *      Matches a value in array    => In
 *      Matches no values in array  => Nin
 *
 *          https://www.mongodb.com/docs/manual/reference/operator/query/eq/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/ne/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/gt/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/gte/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/lt/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/lt/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/nin/
 */
template<typename T>
struct Eq
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Eq(CType init)
        : $eq(std::move(init))
    {}
    T       $eq;
};
template<typename T>
struct Ne
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Ne(CType init)
        : $ne(std::move(init))
    {}
    T       $ne;
};
template<typename T>
struct Gt
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Gt(CType init)
        : $gt(std::move(init))
    {}
    T       $gt;
};
template<typename T>
struct Gte
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Gte(CType init)
        : $gte(std::move(init))
    {}
    T       $gte;
};
template<typename T>
struct Lt
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Lt(CType init)
        : $lt(std::move(init))
    {}
    T       $lt;
};
template<typename T>
struct Lte
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Lte(CType init)
        : $lte(std::move(init))
    {}
    T       $lte;
};
template<typename T>
struct In
{
    using Query = bool;
    using CType = std::initializer_list<T>;
    In(CType init)
        : $in(std::move(init))
    {}
    std::vector<T>  $in;
};
template<typename T>
struct Nin
{
    using Query = bool;
    using CType = std::initializer_list<T>;
    Nin(CType init)
        : $nin(std::move(init))
    {}
    std::vector<T>  $nin;
};

/*
 * query-logical
 * https://www.mongodb.com/docs/manual/reference/operator/query-logical/
 *      Logical AND of two Values           => And
 *      Logical OR  of two Values           => Or
 *      Logical NOR of two Values           => Nor => NOR(A, B) => !(OR(A,B))
 *      Logical NOT                         => Not
 *
 *          https://www.mongodb.com/docs/manual/reference/operator/query/and/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/or/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/nor/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/not/
 */
template<typename LHS, typename RHS>
struct And
{
    using Query = bool;
    using LP = ConstructorType<LHS>;
    using RP = ConstructorType<RHS>;
    using CType = std::pair<LP, RP>;
    And(CType init)
        : $and(std::move(init.first), std::move(init.second))
    {}
    And(LP lhs, RP rhs)
        : $and(std::move(lhs), std::move(rhs))
    {}
    std::tuple<LHS, RHS> $and;
};
template<typename LHS, typename RHS>
struct Or
{
    using Query = bool;
    using LP = ConstructorType<LHS>;
    using RP = ConstructorType<RHS>;
    using CType = std::pair<LP, RP>;
    Or(CType init)
        : $or(std::move(init.first), std::move(init.second))
    {}
    Or(LP lhs, RP rhs)
        : $or(std::move(lhs), std::move(rhs))
    {}
    std::tuple<LHS, RHS>  $or;
};
template<typename LHS, typename RHS>
struct Nor
{
    using Query = bool;
    using LP = ConstructorType<LHS>;
    using RP = ConstructorType<RHS>;
    using CType = std::pair<LP, RP>;
    Nor(CType init)
        : $nor(std::move(init.first), std::move(init.second))
    {}
    Nor(LP lhs, RP rhs)
        : $nor(std::move(lhs), std::move(rhs))
    {}
    std::tuple<LHS, RHS> $nor;
};
template<typename T>
struct Not
{
    using Query = bool;
    using CType = ConstructorType<T>;
    Not(CType init)
        : $not(std::move(init))
    {}
    T       $not;
};
/*
 * query-element
 * https://www.mongodb.com/docs/manual/reference/operator/query-element/
 *      An element exists in the object     =>      Exists
 *          https://www.mongodb.com/docs/manual/reference/operator/query/exists/
 *      An element has a specific Type      =>      Type
 *          https://www.mongodb.com/docs/manual/reference/operator/query/exists/
 */
template<typename T>
struct Exists
{
    using Query = bool;
    using CType = bool;
    Exists(bool init)
        : $exists(init)
    {}
    bool   $exists;
};
template<typename T>
struct Type
{
    using Query = bool;
    using CType = BsonType;
    Type(BsonType type)
        : $type(static_cast<std::int32_t>(type))
    {}
    std::int32_t        $type;
};
/*
 * query-evaluation
 * https://www.mongodb.com/docs/manual/reference/operator/query-evaluation/
 *      Expression:                             =>  TODO
 *          https://www.mongodb.com/docs/manual/reference/operator/query/expr/
 *          I think this is totally possible using the current schema using C++ classes.
 *          BUT. I don;t think that is the best idea.
 *               I want to think about a better way to represent expressions better.
 *      JsonSchema:                             =>  TODO
 *          https://www.mongodb.com/docs/manual/reference/operator/query/jsonSchema/
 *          This I don't quite understand.
 *          So open to suggestions here.
 *      Mod field and test remainder            =>  Mod
 *          https://www.mongodb.com/docs/manual/reference/operator/query/mod/
 *      Regular Expression                      =>  RegEx
 *          https://www.mongodb.com/docs/manual/reference/operator/query/regex/
 *      Text search of whole object             =>  Text
 *          https://www.mongodb.com/docs/manual/reference/operator/query/text/
 *          Can't seem to get his working.
 *      Where uses javascript expression.       => TODO
 *          Don't understand how this works.
 *
 */
// Expr         TODO
// JsonSchema   TODO
template<typename T>
struct Mod
{
    using Query = bool;
    using CType = std::pair<std::uint32_t, std::uint32_t>;
    Mod(CType init)
        : $mod({init.first, init.second})
    {}

    std::array<std::uint32_t, 2>    $mod;
};
template<typename T>
struct RegEx
{
    using Query = bool;
    using CType = std::pair<std::string, std::string>;
    RegEx(CType init)
        : $regex(std::move(init.first))
        , $options(std::move(init.second))
    {}

    std::string     $regex;
    std::string     $options;
};

#if 0
struct TextSearch
{
    using Query = bool;
    std::string                 $search;
    std::optional<std::string>  $language;
    std::optional<bool>         $caseSensitive;
    std::optional<bool>         $diacriticSensitive;
};
struct Text
{
    using Query = bool;
    Text(std::string search, std::optional<std::string> language = {}, std::optional<bool> cs = {}, std::optional<bool> ds = {})
        : $text{std::move(search), std::move(language), std::move(cs), std::move(ds)}
    {}
    TextSearch        $text;
};
#endif
// Where TODO
/*
 * query-array
 *      https://www.mongodb.com/docs/manual/reference/operator/query-array/
 *      Has all the following elements:                                 => All
 *      An array has an element that matches multiple considitions:     => ElemMatch
 *      An array has a specific size:                                   => Size
 *
 *          https://www.mongodb.com/docs/manual/reference/operator/query/all/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/elemMatch/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/size/
 */
template<typename T>
// TODO: require T to be a vector/array etc.
struct All
{
    using Query = bool;
    using CType = ConstructorType<T>;
    All(CType init)
        : $all(std::move(init))
    {}
    T       $all;
};
template<typename T>
struct Elements
{
    using Query = bool;
    std::optional<T>    $eq;
    std::optional<T>    $ne;
    std::optional<T>    $gt;
    std::optional<T>    $gte;
    std::optional<T>    $lt;
    std::optional<T>    $lte;
};

// TODO: Change this needind Elements.
template<typename T>
// TODO: require T to be a vector/array etc.
struct ElemMatch
{
    using Query = bool;
    using FType = ConstructorType<T>;
    using VType = typename FType::value_type;
    using CType = Elements<VType>;
    ElemMatch(CType init)
        : $elemMatch{std::move(init)}
    {}
    Elements<VType>         $elemMatch;
};
template<typename T>
// TODO: require T to be a vector/array etc.
struct Size
{
    using Query = bool;
    using CType = std::uint32_t;
    Size(std::uint32_t init)
        : $size{init}
    {}
    std::uint32_t       $size;
};

/*
 * Bitwise Query operator
 *  https://www.mongodb.com/docs/manual/reference/operator/query-bitwise/
 *      AllClear
 *      AllSet
 *      AnyClear
 *      AnySet
 *
 *
 *          https://www.mongodb.com/docs/manual/reference/operator/query/bitsAllClear/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/bitsAllSet/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/bitsAnyClear/
 *          https://www.mongodb.com/docs/manual/reference/operator/query/bitsAnySet/
 */
template<typename T>
// TODO require T to be numeric or BinData
struct AllClear
{
    using Query = bool;
    using CType = std::uint32_t;
    AllClear(CType init)
        : $bitsAllClear{std::move(init)}
    {}
    std::uint32_t       $bitsAllClear;
};
template<typename T>
// TODO require T to be numeric or BinData
struct AllSet
{
    using Query = bool;
    using CType = std::uint32_t;
    AllSet(CType init)
        : $bitsAllSet{std::move(init)}
    {}
    std::uint32_t       $bitsAllSet;
};
template<typename T>
// TODO require T to be numeric or BinData
struct AnyClear
{
    using Query = bool;
    using CType = std::uint32_t;
    AnyClear(CType init)
        : $bitsAnyClear{std::move(init)}
    {}
    std::uint32_t       $bitsAnyClear;
};
template<typename T>
// TODO require T to be numeric or BinData
struct AnySet
{
    using Query = bool;
    using CType = std::uint32_t;
    AnySet(CType init)
        : $bitsAnySet{std::move(init)}
    {}
    std::uint32_t       $bitsAnySet;
};


}
}

ThorsAnvil_Template_MakeTrait(2, ThorsAnvil::DB::Mongo::Query,              q, limit, collation, hint);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Eq,        $eq);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Ne,        $ne);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Gt,        $gt);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Gte,       $gte);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Lt,        $lt);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Lte,       $lte);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::In,        $in);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Nin,       $nin);
ThorsAnvil_Template_MakeTrait(2, ThorsAnvil::DB::Mongo::QueryOp::And,       $and);
ThorsAnvil_Template_MakeTrait(2, ThorsAnvil::DB::Mongo::QueryOp::Or,        $or);
ThorsAnvil_Template_MakeTrait(2, ThorsAnvil::DB::Mongo::QueryOp::Nor,       $nor);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Not,       $not);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Exists,    $exists);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Type,      $type);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Mod,       $mod);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::RegEx,     $regex, $options);
#if 0
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::TextSearch,$search, $language, $caseSensitive, $diacriticSensitive);
ThorsAnvil_MakeTrait(            ThorsAnvil::DB::Mongo::QueryOp::Text,      $text);
#endif
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::All,       $all);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Elements,  $eq, $ne, $gt, $gte, $lt, $lte);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::ElemMatch, $elemMatch);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::Size,      $size);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::AllClear,  $bitsAllClear);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::AllSet,    $bitsAllSet);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::AnyClear,  $bitsAnyClear);
ThorsAnvil_Template_MakeTrait(1, ThorsAnvil::DB::Mongo::QueryOp::AnySet,    $bitsAnySet);


#endif
