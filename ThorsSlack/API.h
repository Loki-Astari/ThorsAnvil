#ifndef THORSANVIL_SLACK_API_H
#define THORSANVIL_SLACK_API_H

#include "ThorsSlackConfig.h"
#include "SlackBlockKit.h"
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

struct BotIcon
{
    std::string                 image_36;
    std::string                 image_48;
    std::string                 image_72;
};

struct BotProfile
{
    std::string                 id;
    std::string                 app_id;
    std::string                 user_id;
    std::string                 name;
    BotIcon                     icons;
    bool                        deleted;
    std::time_t                 updated;
    std::string                 team_id;
};
using OptBotProfile = std::optional<BotProfile>;

struct Reaction
{
    std::string                 name;
    VecString                   users;
    int                         count;
};
using VecReaction = std::vector<Reaction>;
using OptVecReaction = std::optional<VecReaction>;

struct Message
{
    std::string                 user;
    std::string                 type;
    std::string                 ts;
    OptString                   client_msg_id;
    OptString                   bot_id;
    OptString                   app_id;
    std::string                 text;
    std::string                 team;
    OptString                   thread_ts;
    OptInt                      reply_count;
    OptInt                      reply_users_count;
    OptString                   latest_reply;
    OptVecString                reply_users;
    OptBool                     is_locked;
    OptBool                     subscribed;
    OptBotProfile               bot_profile;
    BlockKit::Blocks            blocks;
    OptVecString                pinned_to;
    OptString                   permalink;
    OptVecReaction              reactions;
};
using OptMessage = std::optional<Message>;
using VecMessage = std::vector<Message>;

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
    API::Message                message;
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
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::BotIcon, image_36, image_48, image_72);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::BotProfile, id, app_id, user_id, name, icons, deleted, updated, team_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Reaction, name, users, count);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Message, user, type, ts, client_msg_id, bot_id, app_id, text, team, thread_ts, reply_count, reply_users_count, latest_reply, reply_users, is_locked, subscribed, bot_profile, blocks, pinned_to, permalink, reactions);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ResponseMetaData, messages);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::OK, ok);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Error, ok, error, needed, provided, errors, warning, warnings, response_metadata);


#endif
