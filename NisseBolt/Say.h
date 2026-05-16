#ifndef  THORSANVIL_NISSE_BOLT_SAY_H
#define  THORSANVIL_NISSE_BOLT_SAY_H

#include "NisseBoltConfig.h"

#include "ThorsSlack/APIChatMessage.h"
#include "ThorsSlack/SlackClient.h"

#include <string>
#include <optional>

namespace ThorsAnvil::Nisse::Bolt
{

using OptString = std::optional<std::string>;

struct Where
{
    std::string             channel;
    OptString               icon_emoji;
    OptString               username;
    OptString               ts;
};

class Say
{
    Slack::SlackClient&     client;
    Where                   where;
    public:
        Say(Slack::SlackClient& client, Where where);
        void operator()(std::string const& message) const;
        void operator()(std::string const& message, Where const& where) const;
        void operator()(Slack::BlockKit::Blocks const&) const;
        void operator()(Slack::BlockKit::Blocks const&, Where const& where) const;
    private:
        void sendMessage(Slack::API::Chat::POSTMessage message) const;
};

}

#endif
