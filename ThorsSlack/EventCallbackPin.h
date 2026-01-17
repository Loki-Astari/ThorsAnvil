#ifndef THORSANVIL_SLACK_EVENT_PIN_ADDED_H
#define THORSANVIL_SLACK_EVENT_PIN_ADDED_H

// Documentation: https://docs.slack.dev/reference/events/reaction_added/

#include "ThorsSlackConfig.h"
#include "API.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>

namespace ThorsAnvil::Slack::Event
{
#if 0
https://docs.slack.dev/reference/events/pin_added
// Pin Added
  "event":
  {
    "type": "pin_added",
    "user": "U095XJHJ1J5",
    "item":
    {
      "type": "message",
      "created": 1764550467,
      "created_by": "U095XJHJ1J5",
      "channel": "<Your Slack Channel>",
      "message":
      {
        "user": "U09S3D8R00Z",
        "type": "message",
        "ts": "1764544528.464179",
        "bot_id": "B09RJ4A000K",
        "app_id": "A09RQFXSEKC",
        "text": "I hope the tour went well, Mr. Wonka.",
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
            "block_id": "BWh",
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
        "permalink": "https://thorsanvilworkspace.slack.com/archives/X09XU2UXXMS/p1764544528464179"
      }
    },
    "item_user": "U09S3D8R00Z",
    "channel_id": "<Your Slack Channel>",
    "pin_count": 1,
    "pinned_info":
    {
      "channel": "<Your Slack Channel>",
      "pinned_by": "U095XJHJ1J5",
      "pinned_ts": 1764550467
    },
    "event_ts": "1764550467.000600"
  },
// https://docs.slack.dev/reference/events/pin_removed
// Pin Removed
  "event":
  {
    "type": "pin_removed",
    "user": "U095XJHJ1J5",
    "item":
    {
      "type": "message",
      "created": 1764550467,
      "created_by": "U095XJHJ1J5",
      "channel": "<Your Slack Channel>",
      "message":
      {
        "user": "U09S3D8R00Z",
        "type": "message",
        "ts": "1764544528.464179",
        "bot_id": "B09RJ4A000K",
        "app_id": "A09RQFXSEKC",
        "text": "I hope the tour went well, Mr. Wonka.",
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
            "block_id": "dmxvA",
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
        "permalink": "https://thorsanvilworkspace.slack.com/archives/X09XU2UXXMS/p1764544528464179"
      }
    },
    "item_user": "U09S3D8R00Z",
    "channel_id": "<Your Slack Channel>",
    "pin_count": 0,
    "pinned_info":
    {
      "channel": "<Your Slack Channel>",
      "pinned_by": "U095XJHJ1J5",
      "pinned_ts": 1764550467
    },
    "has_pins": false,
    "event_ts": "1764550476.000900"
  },
#endif

struct PinnedInfo
{
    std::string                 channel;
    std::string                 pinned_by;
    std::time_t                 pinned_ts;
};

struct PinAdded
{
    // std::string                 type; -- "pin_added"
    std::string                 user;
    API::Item                   item;
    std::string                 item_user;
    std::string                 channel_id;
    int                         pin_count;
    PinnedInfo                  pinned_info;
    std::string                 event_ts;

    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::PinAdded, pin_added);
    ThorsAnvil_TypeFieldName(type);
};

struct PinRemoved
{
    // std::string                 type; -- "pin_removed",
    std::string                 user;
    API::Item                   item;
    std::string                 item_user;
    std::string                 channel_id;
    int                         pin_count;
    PinnedInfo                  pinned_info;
    bool                        has_pins;
    std::string                 event_ts;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::PinRemoved, pin_removed);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::PinnedInfo, channel, pinned_by, pinned_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::PinAdded, user, item, item_user, channel_id, pin_count, pinned_info, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::PinRemoved, user, item, item_user, channel_id, pin_count, pinned_info, has_pins, event_ts);


#endif
