#ifndef THORSANVIL_SLACK_EVENT_USER_H
#define THORSANVIL_SLACK_EVENT_USER_H

// Documentation: https://docs.slack.dev/reference/events/user_change

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/user_change
  "event":
  {
    "type": "user_change",
    "user": { "id": "U1234567", "team_id": "T1234567", "name": "some-user", ... "profile": { ... } ... },
    "cache_ts": 1648596421,
    "event_ts": "1648596712.000001"
  }
// https://docs.slack.dev/reference/events/user_connection
  "event":
  {
    "type": "user_connection",
    "subtype": "connect",
    "user": "U012A3BC4DE",
    "event_ts": "1764264284.841251",
    "trigger_id": "...",
    "enterprise_id": "E012A3BC4DE"
  }
// https://docs.slack.dev/reference/events/user_huddle_changed
  "event":
  {
    "type": "user_huddle_changed",
    "user": { ... },
    "cache_ts": 1648596421,
    "event_ts": "1648596712.000001"
  }
#endif

using OptString = std::optional<std::string>;
using OptBool = std::optional<bool>;
using OptInt = std::optional<int>;
using OptTime = std::optional<std::time_t>;

struct UserProfile
{
    OptString                   title;
    OptString                   phone;
    OptString                   skype;
    OptString                   real_name;
    OptString                   real_name_normalized;
    OptString                   display_name;
    OptString                   display_name_normalized;
    OptString                   status_text;
    OptString                   status_emoji;
    OptInt                      status_expiration;
    OptString                   avatar_hash;
    OptString                   first_name;
    OptString                   last_name;
    OptString                   image_24;
    OptString                   image_32;
    OptString                   image_48;
    OptString                   image_72;
    OptString                   image_192;
    OptString                   image_512;
    OptString                   status_text_canonical;
    OptString                   team;
    OptString                   email;
    OptString                   huddle_state;
    OptTime                     huddle_state_expiration_ts;
    OptString                   huddle_state_call_id;
};

struct UserInfo
{
    std::string                 id;
    std::string                 team_id;
    std::string                 name;
    bool                        deleted;
    OptString                   color;
    OptString                   real_name;
    OptString                   tz;
    OptString                   tz_label;
    OptInt                      tz_offset;
    UserProfile                 profile;
    OptBool                     is_admin;
    OptBool                     is_owner;
    OptBool                     is_primary_owner;
    OptBool                     is_restricted;
    OptBool                     is_ultra_restricted;
    OptBool                     is_bot;
    OptBool                     is_app_user;
    OptTime                     updated;
    OptBool                     is_email_confirmed;
    OptString                   who_can_share_contact_card;
    OptString                   locale;
};

struct UserChange
{
    // std::string                 type;
    UserInfo                    user;
    std::time_t                 cache_ts;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/UserChange"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::UserChange, user_change);
    ThorsAnvil_TypeFieldName(type);
};

struct UserConnection
{
    // std::string                 type;
    std::string                 subtype;
    std::string                 user;
    std::string                 event_ts;
    OptString                   trigger_id;
    OptString                   enterprise_id;
    static std::string const& typeName() {static std::string const name{"Event/UserConnection"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::UserConnection, user_connection);
    ThorsAnvil_TypeFieldName(type);
};

struct UserHuddleChanged
{
    // std::string                 type;
    UserInfo                    user;
    std::time_t                 cache_ts;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/UserHuddleChanged"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::UserHuddleChanged, user_huddle_changed);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::UserProfile, title, phone, skype, real_name, real_name_normalized, display_name, display_name_normalized, status_text, status_emoji, status_expiration, avatar_hash, first_name, last_name, image_24, image_32, image_48, image_72, image_192, image_512, status_text_canonical, team, email, huddle_state, huddle_state_expiration_ts, huddle_state_call_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::UserInfo, id, team_id, name, deleted, color, real_name, tz, tz_label, tz_offset, profile, is_admin, is_owner, is_primary_owner, is_restricted, is_ultra_restricted, is_bot, is_app_user, updated, is_email_confirmed, who_can_share_contact_card, locale);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::UserChange, user, cache_ts, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::UserConnection, subtype, user, event_ts, trigger_id, enterprise_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::UserHuddleChanged, user, cache_ts, event_ts);

#endif
