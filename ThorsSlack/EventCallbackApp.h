#ifndef THORSANVIL_SLACK_EVENT_APP_H
#define THORSANVIL_SLACK_EVENT_APP_H

// Documentation: https://docs.slack.dev/reference/events/app_home_opened

#include "ThorsSlackConfig.h"
#include "SlackBlockKit.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/app_deleted
  "event":
  {
    "type": "app_deleted",
    "app_id": "A015CA1LGHG",
    "app_name": "my-admin-app",
    "app_owner_id": "U013B64J7MSZ",
    "team_id": "E073D7H7BBE",
    "team_domain": "ACME Enterprises",
    "event_ts": "1700001891.279278"
  }
// https://docs.slack.dev/reference/events/app_home_opened
  "event":
  {
    "type": "app_home_opened",
    "user": "U123ABC456",
    "channel": "D123ABC456",
    "event_ts": "1515449522000016",
    "tab": "home",
    "view":
    {
        "id": "V123ABC456",
        "team_id": "T123ABC456",
        "type": "home",
        "blocks": [],
        "private_metadata": "",
        "callback_id": "",
        "hash": "1231232323.12321312",
        "clear_on_close": false,
        "notify_on_close": false,
        "root_view_id": "V123ABC456",
        "app_id": "A123ABC456",
        "external_id": "",
        "app_installed_team_id": "T123ABC456",
        "bot_id": "B123ABC456"
    }
  }
// https://docs.slack.dev/reference/events/app_installed
  "event":
  {
    "type": "app_installed",
    "app_id": "A015CA1LGHG",
    "app_name": "my-admin-app",
    "app_owner_id": "U013B64J7MSZ",
    "user_id": "U013B64J7SZ",
    "team_id": "E073D7H7BBE",
    "team_domain": "ACME Enterprises",
    "event_ts": "1700001891.279278"
  }
// https://docs.slack.dev/reference/events/app_rate_limited
  {
    "token": "Jhj5dZrVaK7ZwHHjRyZWjbDl",
    "type": "app_rate_limited",
    "team_id": "T123456",
    "minute_rate_limited": 1518467820,
    "api_app_id": "A123456"
  }
// https://docs.slack.dev/reference/events/app_requested
  "event":
  {
    "type": "app_requested",
    "app_request": { ... }
  }
// https://docs.slack.dev/reference/events/app_uninstalled_team
  "event":
  {
    "type": "app_uninstalled_team",
    "app_id": "A015CA1LGHG",
    "app_name": "my-admin-app",
    "app_owner_id": "U013B64J7MSZ",
    "user_id": "U013B64J7SZ",
    "team_id": "E073D7H7BBE",
    "team_domain": "ACME Enterprises",
    "event_ts": "1700001891.279278"
  }
// https://docs.slack.dev/reference/events/app_uninstalled
  "event":
  {
    "type": "app_uninstalled"
  }
#endif

using OptString = std::optional<std::string>;

struct AppHomeView
{
    std::string                 id;
    std::string                 team_id;
    std::string                 type;
    BlockKit::Blocks            blocks;
    std::string                 private_metadata;
    std::string                 callback_id;
    std::string                 hash;
    bool                        clear_on_close;
    bool                        notify_on_close;
    std::string                 root_view_id;
    std::string                 app_id;
    std::string                 external_id;
    std::string                 app_installed_team_id;
    std::string                 bot_id;
};

struct AppRequestScope
{
    std::string                 name;
    std::string                 description;
    bool                        is_sensitive;
    std::string                 token_type;
    bool                        is_optional;
    bool                        is_approved;
};
using VecAppRequestScope = std::vector<AppRequestScope>;

struct AppRequestApp
{
    std::string                 id;
    std::string                 name;
    std::string                 description;
    std::string                 help_url;
    std::string                 privacy_policy_url;
    std::string                 app_homepage_url;
    std::string                 app_directory_url;
    bool                        is_granular_bot_app;
    bool                        is_app_directory_approved;
    bool                        is_internal;
    std::string                 additional_info;
};

struct AppRequestPreviousResolution
{
    std::string                 status;
    VecAppRequestScope          scopes;
};
using OptAppRequestPreviousResolution = std::optional<AppRequestPreviousResolution>;

struct AppRequestUser
{
    std::string                 id;
    std::string                 name;
    std::string                 email;
};

struct AppRequestTeam
{
    std::string                 id;
    std::string                 name;
    std::string                 domain;
};

struct AppRequest
{
    std::string                 id;
    AppRequestApp               app;
    OptAppRequestPreviousResolution previous_resolution;
    AppRequestUser              user;
    AppRequestTeam              team;
    VecAppRequestScope          scopes;
    std::string                 message;
};

struct AppDeleted
{
    // std::string                 type;
    std::string                 app_id;
    std::string                 app_name;
    std::string                 app_owner_id;
    std::string                 team_id;
    std::string                 team_domain;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/AppDeleted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AppDeleted, app_deleted);
    ThorsAnvil_TypeFieldName(type);
};

struct AppHomeOpened
{
    // std::string                 type;
    std::string                 user;
    std::string                 channel;
    std::string                 event_ts;
    std::string                 tab;
    AppHomeView                 view;
    static std::string const& typeName() {static std::string const name{"Event/AppHomeOpened"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AppHomeOpened, app_home_opened);
    ThorsAnvil_TypeFieldName(type);
};

struct AppInstalled
{
    // std::string                 type;
    std::string                 app_id;
    std::string                 app_name;
    std::string                 app_owner_id;
    std::string                 user_id;
    std::string                 team_id;
    std::string                 team_domain;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/AppInstalled"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AppInstalled, app_installed);
    ThorsAnvil_TypeFieldName(type);
};

struct AppRateLimited
{
    // std::string                 type;
    std::string                 token;
    std::string                 team_id;
    std::time_t                 minute_rate_limited;
    std::string                 api_app_id;
    static std::string const& typeName() {static std::string const name{"Event/AppRateLimited"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AppRateLimited, app_rate_limited);
    ThorsAnvil_TypeFieldName(type);
};

struct AppRequested
{
    // std::string                 type;
    AppRequest                  app_request;
    static std::string const& typeName() {static std::string const name{"Event/AppRequested"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AppRequested, app_requested);
    ThorsAnvil_TypeFieldName(type);
};

struct AppUninstalledTeam
{
    // std::string                 type;
    std::string                 app_id;
    std::string                 app_name;
    std::string                 app_owner_id;
    std::string                 user_id;
    std::string                 team_id;
    std::string                 team_domain;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/AppUninstalledTeam"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AppUninstalledTeam, app_uninstalled_team);
    ThorsAnvil_TypeFieldName(type);
};

struct AppUninstalled
{
    // std::string                 type;
    static std::string const& typeName() {static std::string const name{"Event/AppUninstalled"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AppUninstalled, app_uninstalled);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppHomeView, id, team_id, type, blocks, private_metadata, callback_id, hash, clear_on_close, notify_on_close, root_view_id, app_id, external_id, app_installed_team_id, bot_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppRequestScope, name, description, is_sensitive, token_type, is_optional, is_approved);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppRequestApp, id, name, description, help_url, privacy_policy_url, app_homepage_url, app_directory_url, is_granular_bot_app, is_app_directory_approved, is_internal, additional_info);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppRequestPreviousResolution, status, scopes);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppRequestUser, id, name, email);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppRequestTeam, id, name, domain);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppRequest, id, app, previous_resolution, user, team, scopes, message);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppDeleted, app_id, app_name, app_owner_id, team_id, team_domain, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppHomeOpened, user, channel, event_ts, tab, view);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppInstalled, app_id, app_name, app_owner_id, user_id, team_id, team_domain, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppRateLimited, token, team_id, minute_rate_limited, api_app_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppRequested, app_request);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppUninstalledTeam, app_id, app_name, app_owner_id, user_id, team_id, team_domain, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppUninstalled);

#endif
