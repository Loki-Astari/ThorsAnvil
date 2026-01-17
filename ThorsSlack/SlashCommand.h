#ifndef THORSANVIL_SLACK_SLASH_COMMAND_H
#define THORSANVIL_SLACK_SLASH_COMMAND_H

#include <string>
#include <NisseHTTP/Request.h>

namespace ThorsAnvil::Slack
{

/*
 * This object is constructed from application/x-www-form-urlencoded encoded parameters.
 *
 * Nisse extracts these values into the variables map.
 *      token => 4axIIPlMUVqU5NWGL7OQOfO9
 *      command => /todo
 *      text => with values
 *      response_url => https://hooks.slack.com/commands/T095XJHH589/10196241860161/44ZP0bgd65ZsQnmw5WEp3Jch
 *      trigger_id => 10211624935328.9201629583281.40a29da9a8d8df9c7dcd90ea912dd05f
 *      user_id => U095XJHJ1J5
 *      user_name => martin.york
 *      team_id => T095XJHH589
 *      channel_id => C09RU2URYMS
 *      api_app_id => A09RQFXSEKC
 *
 * The etc part?
 *      channel_name => genreral
 *      is_enterprise_install => false
 *      team_domain => thorsanvilworkspace
 * Other Values:
 *      accept => <star>/<star>
 *      accept-encoding => deflate
 *      content-length => 438
 *      content-type => application/x-www-form-urlencoded
 *      host => d183b6bbf2c4.ngrok-free.app
 *      user-agent => Slackbot 1.0 (+https://api.slack.com/robots)
 *      x-forwarded-for => 3.226.240.182
 *      x-forwarded-host => d183b6bbf2c4.ngrok-free.app
 *      x-forwarded-proto => https
 *      x-slack-request-timestamp => 1766560837
 *      x-slack-signature => v0=37df9211b5a7712ee2b828513e76a9bd1262f9c5f71d3d2a2f329d7eeca8a557
 */
struct SlashCommand
{
    // https://docs.slack.dev/interactivity/implementing-slash-commands/
    std::string         token;          // (Deprecated) This is a verification token, a deprecated feature that you shouldn't use any more. It was used to verify that requests were legitimately being sent by Slack to your app, but you should use the signed secrets functionality to do this instead.
    std::string         command;        // The command that was entered to trigger this request. This value can be useful if you want to use a single Request URL to service multiple slash commands, as it allows you to tell them apart.
    std::string         text;           // This is the part of the slash command after the command itself, and it can contain absolutely anything the user might decide to type. It is common to use this text parameter to provide extra context for the command. You can prompt users to adhere to a particular format by showing them in the Usage Hint field when creating a command.
    std::string         response_url;   // A temporary webhook URL that you can use to generate message responses.
    std::string         trigger_id;     // A short-lived ID that will allow your app to open a modal.
    std::string         user_id;        // The ID of the user who triggered the command.
    std::string         user_name;      // (Deprecated) The plain text name of the user who triggered the command. Do not rely on this field as it has been phased out. Use the user_id instead.
    // These IDs provide context about where the user was in Slack when they triggered your app's command (e.g. the workspace, Enterprise organization, or channel). You may need these IDs for your command response. The various accompanying *_name values provide you with the plain text names for these IDs, but as always you should only rely on the IDs as the names might change arbitrarily. We'll include enterprise_id and enterprise_name parameters on command invocations when the executing workspace is part of an Enterprise organization.
    std::string         team_id;        //
    std::string         enterprise_id;
    std::string         channel_id;
    // etc.
    std::string         api_app_id;     // Your Slack app's unique identifier. Use this in conjunction with request signing to verify context for inbound requests.

    SlashCommand(ThorsAnvil::Nisse::HTTP::Request const& request)
        : token(request.variables()["token"])
        , command(request.variables()["command"])
        , text(request.variables()["text"])
        , response_url(request.variables()["response_url"])
        , trigger_id(request.variables()["trigger_id"])
        , user_id(request.variables()["user_id"])
        , user_name(request.variables()["user_name"])
        , team_id(request.variables()["team_id"])
        , enterprise_id(request.variables()["enterprise_id"])
        , channel_id(request.variables()["channel_id"])
        , api_app_id(request.variables()["api_app_id"])
    {}


};


}

#endif
