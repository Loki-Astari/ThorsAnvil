#ifndef THORSANVIL_SLACK_API_H
#define THORSANVIL_SLACK_API_H

#include "ThorsSlackConfig.h"
#include "BlockKit.h"
#include "EventMessage.h"
#include "NisseHTTP/Util.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"

#include <ctime>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
#include <optional>
#include <type_traits>

namespace ThorsAnvil::Slack::API
{

enum class Scope {Bot, User};

using OptBool       = std::optional<bool>;
using OptInt        = std::optional<int>;
using OptTime       = std::optional<std::time_t>;
using OptString     = std::optional<std::string>;
using VecString     = std::vector<std::string>;
using OptVecString  = std::optional<std::vector<std::string>>;
using Method        = ThorsAnvil::Nisse::HTTP::Method;

struct Cursor
{
    std::string                         next_cursor;
};
using OptCursor = std::optional<Cursor>;

struct ResponseMetaData
{
    VecString                   messages;
};
using OptResponseMetaData = std::optional<ResponseMetaData>;

struct Item
{
    std::string                 type;
    OptTime                     created;
    OptString                   created_by;
    std::string                 channel;
    Event::Message              message;
};
using VecItems = std::vector<Item>;

struct ListReply
{
    bool                        ok      = false;
    VecItems                    items;
    OptCursor                   response_metadata;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Pin::ListReply);
};

struct OK
{
    bool                        ok;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::OK);
};

struct Error
{
    bool                        ok;
    OptString                   needed;
    OptString                   provided;
    OptString                   error;
    OptVecString                errors;
    OptString                   warning;
    OptVecString                warnings;
    OptResponseMetaData         response_metadata;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Error);
};

struct Debug
{
    bool                        ok;
};

// Primary template: By default, a type is not std::optional
template<typename>
constexpr bool is_optional_impl = false;

// Partial specialization for std::optional<T>: This will be chosen when the type is std::optional
template<typename T>
constexpr bool is_optional_impl<std::optional<T>> = true;

// Helper variable template to handle cv-qualifiers and references
template<typename T>
constexpr bool is_optional = is_optional_impl<std::remove_cvref_t<T>>; // C++20 and later

template<typename T>
void buildQueryE(std::stringstream& stream, std::string& sep, std::string_view name, T const& val)
{
    // TODO
    // Should probably URL-encode the name and value.
    if constexpr (is_optional<T>) {
        if (val.has_value()) {
            stream << sep << name << "=" << val.value();
            sep = "&";
        }
    }
    else {
        stream << sep << name << "=" << val;
        sep = "&";
    }
}

template<typename T, typename F>
void buildQueryD(std::stringstream& stream, std::string& sep, T const& val, F const& field)
{
    buildQueryE(stream, sep, field.first, (val.*(field.second)));
}

template<typename T, typename M, std::size_t... I>
std::string buildQueryC(T const& val, M const& fields, std::index_sequence<I...> const&)
{
    std::stringstream       query;
    std::string             sep = "";

    (buildQueryD(query, sep, val, std::get<I>(fields)), ...);
    return query.str();
}

template<typename T, typename M>
std::string buildQueryB(T const& val, M const& fields)
{
    return buildQueryC(val, fields, std::make_index_sequence<std::tuple_size<M>::value>{});
}

template<typename T>
std::string buildQueryA(T const& val)
{
    return buildQueryB(val, ThorsAnvil::Serialize::Traits<T>::getMembers());
}


}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Cursor, next_cursor);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Item, type, created, created_by, channel, message);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ListReply, ok, items, response_metadata);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ResponseMetaData, messages);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::OK, ok);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Error, ok, error, needed, provided, errors, warning, warnings, response_metadata);


#endif
