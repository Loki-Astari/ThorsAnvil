#ifndef THORSANVIL_SLACK_API_STAR_H
#define THORSANVIL_SLACK_API_STAR_H


#include "ThorsSlackConfig.h"
#include "API.h"

namespace ThorsAnvil::Slack::API::Star
{

#if 0
Documentation: https://docs.slack.dev/reference/methods/stars.add/
Response:
{"ok":true}
Documentation: https://docs.slack.dev/reference/methods/stars.remove/
Response:
{"ok":true}
Documentation: https://docs.slack.dev/reference/methods/stars.list/
Response:
{
  "ok": true,
  "items": [
    {
      "type": "message",
      "channel": "<Your Slack Channel>",
      "message":
      {
        "user": "U09S3D8R00Z",
        "type": "message",
        "ts": "1764530084.305729",
        "bot_id": "B09RJ4A000K",
        "app_id": "A09RQFXSEKC",
        "text": "The APIStarTest::Add message With star",
        "team": "T095XJHH589",
        "bot_profile":
        {
          "id": "B09RJ4A000K",
          "deleted": false,
          "name": "ThorsSlackBotOne",
          "updated": 1762644664,
          "app_id": "A09RQFXSEKC",
          "user_id": "U09S3D8R00Z",
          "icons":
          {
            "image_36": "https://a.slack-edge.com/80588/img/plugins/app/bot_36.png",
            "image_48": "https://a.slack-edge.com/80588/img/plugins/app/bot_48.png",
            "image_72": "https://a.slack-edge.com/80588/img/plugins/app/service_72.png"
          },
          "team_id": "T095XJHH589"
        },
        "blocks": [
          {
            "type": "rich_text",
            "block_id": "7sDnr",
            "elements": [
              {
                "type": "rich_text_section",
                "elements": [
                  {
                    "type": "text",
                    "text": "The APIStarTest::Add message With star"
                  }
                ]
              }
            ]
          }
        ],
        "permalink": "https://thorsanvilworkspace.slack.com/archives/X09XU2UXXMS/p1764530084305729"
      },
      "date_create": 1764530084
    }
  ],
  "response_metadata":
  {
    "next_cursor": ""
  }
}

#endif

// Response Objects

// Action Objects
struct Add
{
    static constexpr char const* api = "https://slack.com/api/stars.add";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::User;
    using Reply = API::OK;

    OptString                       channel;            // Channel to add star to, or channel where the message to add star to was posted (used with timestamp).
    OptString                       file;               // File to add star to.
    OptString                       file_comment;       // File comment to add star to.
    OptString                       timestamp;          // Timestamp of the message to add star to.
};

struct Remove
{
    static constexpr char const* api = "https://slack.com/api/stars.remove";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::User;
    using Reply = API::OK;

    OptString                       channel;            // Channel to remove star from, or channel where the message to remove star from was posted (used with timestamp).
    OptString                       file;               // File to remove star from.
    OptString                       file_comment;       // File comment to remove star from.
    OptString                       timestamp;          // Timestamp of the message to remove star from.
};
struct List
{
    static constexpr char const* api = "https://slack.com/api/stars.list";
    static constexpr Method method = Method::GET;
    static constexpr Scope  scope = Scope::User;
    using Reply = API::ListReply;

    OptInt                          count;              // Default: 100
    OptString                       cursor;             // Parameter for pagination. Set cursor equal to the next_cursor attribute returned by the previous request's response_metadata. This parameter is optional, but pagination is mandatory: the default value simply fetches the first "page" of the collection. See pagination for more details.
    OptInt                          limit;              // The maximum number of items to return. Fewer than the requested number of items may be returned, even if the end of the list hasn't been reached.
    OptInt                          page;               // Default: 1
    OptString                       team_id;            // encoded team id to list stars in, required if org token is used
};


}

// Response objects

// Action objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Star::Add, channel, file, file_comment, timestamp);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Star::Remove, channel, file, file_comment, timestamp);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Star::List, count, cursor, limit, page, team_id);

#endif
