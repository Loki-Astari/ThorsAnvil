#ifndef THORSANVIL_DB_MONGO_MONGO_UPDATE_H
#define THORSANVIL_DB_MONGO_MONGO_UPDATE_H

#include "ThorSerialize/Traits.h"
#include "ThorsMongoConfig.h"
#include "MongoActions.h"
#include "ThorsMongoCommon.h"
#include <iterator>

/*
 *
 * Usage expamples below assume the following:
 *  struct Person
 *  {
 *      std::string                 name;
 *      std::uint32_t               age;
 *      DateTime                    bDay;
 *      std::vector<std::string>    food;
 *  };
 *  template<typename T>
 *  struct AgeField
 *  {
 *      using CType = ConstructorType<T>;
 *      T       age;
 *  };
 *  template<typename T>
 *  struct BDayField
 *  {
 *      using CType = ConstructorType<T>;
 *      T       bDay;
 *  };
 *  template<typenam T>
 *  struct FoodField
 *  {
 *      using CType = ConstructorType<T>;
 *      T       food;
 *  };
 *  using FindByAge = AgeField<Eq>
 *
 * Update Operators:
 *      CurrentDate
 *      Inc
 *      Min
 *      Max
 *      Mul
 *      Set
 */

namespace ThorsAnvil::DB::Mongo
{
namespace QueryOp
{

/*
 * Current Date
 * Usage:
 *      using Update = CurrentDate<BDayField<SetDate>>;
 *      // using Update = CurrentDate<BDayField<SetTimeStamp>>;
 *      // using Update = CurrentDate<BDayField<bool>>;
 *      mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{});    // Set bDat to today.
 */
struct SetDate
{
    SetDate(bool){}
    std::string $type = "date";
};
struct SetTimeStamp
{
    SetTimeStamp(bool){}
    std::string $type = "timestamp";
};

template<typename T>
struct CurrentDate
{
    using CType = void;
    CurrentDate() : $currentDate{true}   {}
    T       $currentDate;
};
template<typename T>
using CurrentDateParam = bool;
template<typename T>
using CurrentDateSetDate = CurrentDate<T>;
template<typename T>
using CurrentDateSetDateParam = SetDate;
template<typename T>
using CurrentDateSetTimeStamp = CurrentDate<T>;
template<typename T>
using CurrentDateSetTimeStampParam = SetTimeStamp;

/*
 *  using Update    = Inc<AgeField<std::uint32_t>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{2});   // Increase age by 2.
 */
template<typename T>
struct Inc
{
    using CType = ConstructorType<T>;
    Inc(CType init): $inc(init)     {}
    T                   $inc;
};
template<typename... A>
struct Inc<Actions<A...>>
{
    Inc(A&&... arg): $inc{std::move(arg)...}    {}
    Actions<A...>       $inc;
};
template<typename T>
using IncParam = std::uint32_t;

/*
 * Usage
 *  using Update = Min<AgeField<std::uint32_t>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{12});   // Set Age to the min of 12 or currentValue
 */
template<typename T>
struct Min
{
    using CType = ConstructorType<T>;
    Min(CType init): $min(init)     {}
    T                   $min;
};
template<typename... A>
struct Min<Actions<A...>>
{
    Min(A&&... arg): $min{std::move(arg)...}    {}
    Actions<A...>       $min;
};
template<typename T>
using MinParam = T;

/*
 * Usage
 *  using Update = Max<AgeField<std::uint32_t>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{12});   // Set Age to the max of 12 or currentValue
 */
template<typename T>
struct Max
{
    using CType = ConstructorType<T>;
    Max(CType init): $max(init)     {}
    T                   $max;
};
template<typename... A>
struct Max<Actions<A...>>
{
    Max(A&&... arg): $max{std::move(arg)...}    {}
    Actions<A...>       $max;
};
template<typename T>
using MaxParam = T;

/*
 * Usage
 *  using Update = Mul<AgeField<std::uint32_t>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{2.6});   // Multiple the age by 2.6
 */
template<typename T>
struct Mul
{
    using CType = ConstructorType<T>;
    Mul(CType init): $mul(init)     {}
    T                   $mul;
};
template<typename... A>
struct Mul<Actions<A...>>
{
    Mul(A&&... arg): $mul{std::move(arg)...}    {}
    Actions<A...>       $mul;
};
template<typename T>
using MulParam = std::uint32_t;

/*
 * Usage
 *  using Update = Rename<AgeField>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{"originalAge"});   // rename the age field to "originalAge"
 */
template<typename T>
struct Rename
{
    using CType = ConstructorType<T>;
    Rename(CType init): $rename(init)     {}
    T           $rename;
};
template<typename... A>
struct Rename<Actions<A...>>
{
    Rename(A&&... arg): $rename{std::move(arg)...}    {}
    Actions<A...>       $rename;
};
template<typename T>
using RenameParam = std::string;

/*
 * Usage:
 *  using Update = Set<AgeField<std::uint32>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{45});   // Set age to 45
 */
template<typename T>
struct Set
{
    using CType = ConstructorType<T>;
    Set(CType init): $set(std::move(init))     {}
    T           $set;
};
template<typename... A>
struct Set<Actions<A...>>
{
    Set(A&&... arg): $set{std::move(arg)...}    {}
    Actions<A...>       $set;
};
template<typename T>
using SetParam = T;

/*
 * Usage:
 *  using Update = SetOnInsert<AgeField<std::uint32>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{45});   // Set age to 45
 */
template<typename T>
struct SetOnInsert
{
    using CType = ConstructorType<T>;
    SetOnInsert(CType init): $setOnInsert(std::move(init))     {}
    T           $setOnInsert;
};
template<typename... A>
struct SetOnInsert<Actions<A...>>
{
    SetOnInsert(A&&... arg): $setOnInsert{std::move(arg)...}    {}
    Actions<A...>       $setOnInsert;
};
template<typename T>
using SetOnInsertParam = T;

/*
 * Usage:
 *  using Update = Unset<AgeField<std::string>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{});   // Removed the age field from the object
 */
template<typename T>
struct Unset
{
    using CType = void;
    Unset(): $unset(std::string("")) {}
    T  $unset;
};
template<typename... A>
struct Unset<Actions<A...>>
{
    Unset(A&&... arg): $unset{std::move(arg)...}    {}
    Actions<A...>       $unset;
};
template<typename T>
using UnsetParam = std::string;

// TODO
//          $(update)
//          $[]
//          $[<identifier>]

/*
 * Usage:
 *  using Update = AddToSet<FoodField<std::string>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{"Ham"});   // Add a value to an array
 */
template<typename T>
struct AddToSet
{
    using CType = ConstructorType<T>;
    AddToSet(CType init): $addToSet(std::move(init)) {}
    T           $addToSet;
};
template<typename... A>
struct AddToSet<Actions<A...>>
{
    AddToSet(A&&... arg): $addToSet{std::move(arg)...}    {}
    Actions<A...>       $addToSet;
};
template<typename T>
using AddToSetParam = typename T::value_type;

/*
 * Usage:
 *  using Update = PopFront<FoodField<std::int32_t>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{});   // Pop a value from front of an array.
 */
template< typename T>
struct PopFront
{
    using CType = void;
    PopFront(): $pop(-1) {}
    T           $pop;
};
template<typename T>
using PopFrontParam = std::int32_t;

/*
 * Usage:
 *  using Update = PopBack<FoodField<std::int32_t>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{});   // Pop a value from back of an array
 */
template<typename T>
struct PopBack
{
    using CType = void;
    PopBack(): $pop(1) {}
    T           $pop;
};
template<typename T>
using PopBackParam = std::int32_t;

/*
 * Usage:
 *  using Update = Pull<FoodField<std::string>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{"Ham"});   // Removed all values that match the input
 *
 * or (You can use conditionals)
 *  using Update = Pull<FoodField<Lt<std::string>>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{"Ham"});   // Removed all values that match the query
 */
template<typename T>
struct Pull
{
    using CType = ConstructorType<T>;
    Pull(CType init): $pull(std::move(init)) {}
    T           $pull;
};
template<typename... A>
struct Pull<Actions<A...>>
{
    Pull(A&&... arg): $pull{std::move(arg)...}    {}
    Actions<A...>           $pull;
};
template<typename T>
using PullParam = typename T::value_type;

/*
 * Usage:
 *  using Update = Push<FoodField<std::string>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{"Ham"});   // Adds a value to the array
 */
template<typename T>
struct Push
{
    using CType = ConstructorType<T>;
    Push(CType init): $push(std::move(init)) {}
    T           $push;
};
template<typename... A>
struct Push<Actions<A...>>
{
    Push(A&&... arg): $push{std::move(arg)...}    {}
    Actions<A...>           $push;
};
template<typename T>
using PushParam = typename T::value_type;

/*
 * Usage:
 *  using Update = PullAll<FoodField<std::vector<std::string>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{{"Ham", "Jam", "Honey"}});   // Removes all values from the input
 */
template<typename T>
struct PullAll
{
    using CType = ConstructorType<T>;
    PullAll(CType init): $pullAll(std::move(init)) {}
    T             $pullAll;
};
template<typename... A>
struct PullAll<Actions<A...>>
{
    PullAll(A&&... arg): $pullAll{std::move(arg)...}    {}
    Actions<A...>           $pullAll;
};
template<typename T>
using PullAllParam = std::vector<typename T::value_type>;

/*
 * Usage:
 *  using Update = AddToSet<FoodField<Each<std::vector<std::string>>>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{{"Ham", "Jam", "Honey"}});   // All the items from a vector added into the array in the object
 */
template<typename T>
struct Each
{
    using CType = T;
    Each(CType init): $each(std::move(init)) {}
    T          $each;
};
template<typename... A>
struct Each<Actions<A...>>
{
    Each(A&&... arg): $each{std::move(arg)...}    {}
    Actions<A...>           $each;
};
template<typename T>
using EachParam = std::vector<typename T::value_type>;

/*
 * Usage:
 *  using Update = Push<FoodField<Position<std::vector<std::string>>>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{{{"Ham", "Jam"}, 23}});   // All the items from a vector added into the array at a specific position
 */
template<typename T>
struct Position
{
    using CType = std::pair<T, std::int32_t>;
    Position(CType init): $each(std::move(init.first)), $position(init.second) {}
    T                       $each;
    std::int32_t            $position;
};
template<typename T>
using PositionParam = std::vector<typename T::value_type>;

/*
 * Usage:
    using Update = Push<FoodField<Slice<std::vector<std::string>>>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{{{"Ham", "Jam"}, 23}});   // Copies vector into array (see documentation https://www.mongodb.com/docs/manual/reference/operator/update/slice/)
 */
template<typename T>
struct Slice
{
    using CType = std::pair<T, std::int32_t>;
    Slice(CType init): $each(std::move(init.first)), $slice(init.second) {}
    T                       $each;
    std::int32_t            $slice;
};
template<typename T>
using SliceParam = std::vector<typename T::value_type>;

/*
 * Usage:
 *  using Update = Push<FoodField<Sort<std::vector<std::string>>>>;
 *  mongo["db"]["collection"].findAndUpdateOne<People>(FindAgeEq{32}, Update{{{"Ham", "Jam"}, SortOrder::Ascending}});   // Sorts the input vector before adding into the destination array.
 */
template<typename T>
struct Sort
{
    using CType = std::pair<T, SortOrder>;
    Sort(CType init): $each(std::move(init.first)), $sort(static_cast<std::int32_t>(init.second)) {}
    T                       $each;
    std::int32_t            $sort;
};
template<typename T>
using SortParam = std::vector<typename T::value_type>;

}
}

ThorsAnvil_MakeTrait(               ThorsAnvil::DB::Mongo::QueryOp::SetDate,            $type);
ThorsAnvil_MakeTrait(               ThorsAnvil::DB::Mongo::QueryOp::SetTimeStamp,       $type);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::CurrentDate,        $currentDate);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Inc,                $inc);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Min,                $min);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Max,                $max);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Mul,                $mul);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Rename,             $rename);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Set,                $set);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::SetOnInsert,        $setOnInsert);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Unset,              $unset);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::AddToSet,           $addToSet);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::PopFront,           $pop);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::PopBack,            $pop);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Pull,               $pull);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Push,               $push);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::PullAll,            $pullAll);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Each,               $each);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Position,           $each, $position);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Slice,              $each, $slice);
ThorsAnvil_Template_MakeTrait(1,    ThorsAnvil::DB::Mongo::QueryOp::Sort,               $each, $sort);

#endif
