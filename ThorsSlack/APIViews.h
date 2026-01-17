#ifndef THORSANVIL_SLACK_API_VIEWS_H
#define THORSANVIL_SLACK_API_VIEWS_H


#include <string>
#include "API.h"
#include "ThorsSlackConfig.h"
#include "APIBlockActions.h"
#include "EventCallback.h"

namespace ThorsAnvil::Slack::API::Views
{

#if 0
Documentation: https://docs.slack.dev/reference/methods/chat.postmessage
{
  "ok": true,
  "view":
  {
    "id": "V0A5UAULH8W",
    "team_id": "T095XJHH589",
    "type": "modal",
    "blocks": [
      {
        "type": "rich_text",
        "block_id": "Db8C2",
        "elements": [
          {
            "type": "rich_text_section",
            "elements": [
              {
                "type": "text",
                "text": "This is a Menu"
              },
              {
                "type": "text",
                "text": "Lets Add Some Text"
              }
            ]
          }
        ]
      }
    ],
    "private_metadata": "",
    "callback_id": "",
    "state":
    {
      "values":
      {
        "lGTo9":
        {
          "XTFuu":
          {
            "type": "plain_text_input",
            "value": "Time to Quit"
          }
        }
      }
    },
    "hash": "1766968411.c27rxSzG",
    "title":
    {
      "type": "plain_text",
      "text": "Slack Menu",
      "emoji": true
    },
    "clear_on_close": false,
    "notify_on_close": false,
    "close": null,
    "submit": null,
    "previous_view_id": null,
    "root_view_id": "V0A5UAULH8W",
    "app_id": "A09RQFXSEKC",
    "external_id": "",
    "app_installed_team_id": "T095XJHH589",
    "bot_id": "B09RJ4A000K"
  }
}
#endif

struct View
{
    // https://docs.slack.dev/reference/views/modal-views
    // std::string                        type;               // The type of view. Set to modal for modals.
    BlockKit::ElTextPlain               title;              // The title that appears in the top-left of the modal. Must be a plain_text text element with a max length of 24 characters.
    BlockKit::Blocks                    blocks;             // An array of blocks that defines the content of the view. Max of 100 blocks.
    BlockKit::OptElTextPlain            close;              // A plain_text element that defines the text displayed in the close button at the bottom-right of the view. Max length of 24 characters.
    BlockKit::OptElTextPlain            submit;             // A plain_text element that defines the text displayed in the submit button at the bottom-right of the view. submit is required when an input block is within the blocks array. Max length of 24 characters.
    OptString                           private_metadata;   // A string that will be sent to your app in view_submission and block_actions events. Max length of 3000 characters.
    OptString                           callback_id;        // An identifier to recognize interactions and submissions of this particular view. Don't use this to store sensitive information (use private_metadata instead). Max length of 255 characters.
    OptBool                             clear_on_close;     // When set to true, clicking on the close button will clear all views in a modal and close it. Defaults to false.
    OptBool                             notify_on_close;    // Indicates whether Slack will send your request URL a view_closed event when a user clicks the close button. Defaults to false.
    OptString                           external_id;        // A custom identifier that must be unique for all views on a per-team basis.
    OptBool                             submit_disabled;    // When set to true, disables the submit button until the user has completed one or more inputs. This property is for configuration modals.
                                                            //
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::View, modal);
    ThorsAnvil_TypeFieldName(type);
};

using UPtrElTextPlain = std::unique_ptr<BlockKit::ElTextPlain>;
using UPtrString      = std::unique_ptr<std::string>;

struct ViewReply
{
    std::string                         id;
    std::string                         team_id;
    std::string                         type;
    BlockKit::ElTextPlain               title;
    BlockKit::Blocks                    blocks;
    UPtrElTextPlain                     close;
    UPtrElTextPlain                     submit;
    std::string                         private_metadata;
    std::string                         callback_id;
    bool                                clear_on_close;
    bool                                notify_on_close;
    std::string                         external_id;
    OptBool                             submit_disabled;
    UPtrString                          previous_view_id;
    SlackState                          state;
    std::string                         hash;
    std::string                         root_view_id;
    std::string                         app_id;
    std::string                         app_installed_team_id;
    std::string                         bot_id;
};


struct OpenReply
{
    bool                                ok;
    ViewReply                           view;
};

struct Open
{
    // https://docs.slack.dev/reference/methods/views.open/
    static constexpr char const* api = "https://slack.com/api/views.open";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = OpenReply;

    View                                view;               // A view payload. This must be a JSON-encoded string.
    OptString                           trigger_id;         // Exchange a trigger to post to the user. Example: 12345.98765.abcd2358fdea
    OptString                           interactivity_pointer;  // Exchange an interactivity pointer to post to the user. Example: 12345.98765.abcd2358fdea
};

struct Publish
{
    // https://docs.slack.dev/reference/methods/views.publish
    static constexpr char const* api = "https://slack.com/api/views.publish";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = API::OK;

    std::string                         user_id;            // id of the user you want publish a view to.
    View                                view;               // A view payload. This must be a JSON-encoded string.
    OptString                           hash;               // A string that represents view state to protect against possible race conditions.
};

struct Push
{
    // https://docs.slack.dev/reference/methods/views.push
    static constexpr char const* api = "https://slack.com/api/views.push";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = API::OK;

    View                                view;               // A view payload. This must be a JSON-encoded string.
    OptString                           trigger_id;         // Exchange a trigger to post to the user. Example: 12345.98765.abcd2358fdea
    OptString                           interactivity_pointer;  // Exchange an interactivity pointer to post to the user. Example: 12345.98765.abcd2358fdea
};

struct Update
{
    // https://docs.slack.dev/reference/methods/views.update
    static constexpr char const* api = "https://slack.com/api/views.update";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = API::OK;

    View                                view;               // A view object. This must be a JSON-encoded string.
    OptString                           view_id;            // A unique identifier of the view to be updated. Either view_id or external_id is required.
    OptString                           external_id;        // A unique identifier of the view set by the developer. Must be unique for all views on a team. Max length of 255 characters. Either view_id or external_id is required.
    OptString                           hash;               // A string that represents view state to protect against possible race conditions.
};


struct ViewSubmission
{
    //std::string                         type;               // view_submission
    SlackTeam                           team;
    SlackUser                           user;
    std::string                         api_app_id;
    std::string                         token;
    std::string                         trigger_id;
    ViewReply                           view;
    std::vector<std::string>            response_urls;
    bool                                is_enterprise_install;
    Event::Enterprise*                  enterprise;

    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ViewSubmission, view_submission);
    ThorsAnvil_TypeFieldName(type);
};
}

// Response objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Views::ViewReply, id, team_id, type, title, blocks, close, submit, private_metadata, callback_id, clear_on_close, notify_on_close, external_id, submit_disabled, previous_view_id, state, hash, root_view_id, app_id, app_installed_team_id, bot_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Views::OpenReply, ok, view);


// Action objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Views::View, title, blocks, close, submit, private_metadata, callback_id, clear_on_close, notify_on_close, external_id, submit_disabled);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Views::Open, view, trigger_id, interactivity_pointer);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Views::Publish,user_id, view, hash);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Views::Push, view, trigger_id, interactivity_pointer);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Views::Update, view, view_id, external_id, hash);

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Views::ViewSubmission, team, user, api_app_id, token, trigger_id, view, response_urls, is_enterprise_install, enterprise);

#endif
