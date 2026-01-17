#ifndef THORSANVIL_SLACK_API_PINS_H
#define THORSANVIL_SLACK_API_PINS_H


#include "ThorsSlackConfig.h"
#include "API.h"
#include <ctime>

namespace ThorsAnvil::Slack::API::Pins
{

#if 0
Documentation: https://docs.slack.dev/reference/methods/pins.add/
Response:
{"ok":true}
Documentation: https://docs.slack.dev/reference/methods/pins.remove/
Response:
{"ok":true}
Documentation: https://docs.slack.dev/reference/methods/pins.list/
Response:
{
  "ok": true,
  "items": [
    {
      "type": "message",
      "created": 1764228505,
      "created_by": "U09S3D8R00Z",
      "channel": "<Your Slack Channel>",
      "message":
      {
        "user": "U09S3D8R00Z",
        "type": "message",
        "ts": "1764228505.264229",
        "bot_id": "B09RJ4A000K",
        "app_id": "A09RQFXSEKC",
        "text": "I hope the tour went well, Mr. Wonka.",
        "team": "T095XJHH589",
        "bot_profile":
        {
          "id": "B09RJ4A000K",
          "app_id": "A09RQFXSEKC",
          "user_id": "U09S3D8R00Z",
          "name": "ThorsSlackBotOne",
          "icons":
          {
            "image_36": "https://a.slack-edge.com/80588/img/plugins/app/bot_36.png",
            "image_48": "https://a.slack-edge.com/80588/img/plugins/app/bot_48.png",
            "image_72": "https://a.slack-edge.com/80588/img/plugins/app/service_72.png"
          },
          "deleted": false,
          "updated": 1762644664,
          "team_id": "T095XJHH589"
        },
        "blocks": [
          {
            "type": "rich_text",
            "block_id": "LNWg",
            "elements": [
              {
                "type": "rich_text_section",
                "elements": [
                  {
                    "type": "text",
                    "text": "I hope the tour went well, Mr. Wonka."
                  }
                ]
              }
            ]
          }
        ],
        "pinned_to": [
          "<Your Slack Channel>"
        ],
        "permalink": "https://thorsanvilworkspace.slack.com/archives/X09XU2UXXMS/p1764228505264229"
      }
    },
  ]
}
#endif

// Response Objects

// Action Objects
struct Add
{
    static constexpr char const* api = "https://slack.com/api/pins.add";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = API::OK;

    std::string                 channel;            // Channel to pin the messsage to. You must also include a timestamp when pinning messages.
    OptString                   timestamp;          // Timestamp of the message to pin. You must also include the channel.
};

struct Remove
{
    static constexpr char const* api = "https://slack.com/api/pins.remove";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = API::OK;

    std::string                 channel;            // Channel to pin the messsage to. You must also include a timestamp when pinning messages.
    OptString                   timestamp;          // Timestamp of the message to pin. You must also include the channel.
};

struct List
{
    static constexpr char const* api = "https://slack.com/api/pins.list";
    static constexpr Method method = Method::GET;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = API::ListReply;

    std::string                 channel;        // Channel to get pinned items for.
};

}

// Response objects

// Action objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Pins::Add, channel, timestamp);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Pins::Remove, channel, timestamp);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Pins::List, channel);

#endif
