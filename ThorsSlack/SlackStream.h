#ifndef THORSANVIL_SLACK_SLACKSTREAM_H
#define THORSANVIL_SLACK_SLACKSTREAM_H

#include "ThorsSlackConfig.h"
#include "NisseHTTP/ClientStream.h"

namespace ThorsAnvil::Slack
{

class SlackStream: public ThorsAnvil::Nisse::HTTP::ClientStream
{
    public:
        SlackStream()
            : ThorsAnvil::Nisse::HTTP::ClientStream("slack.com")
        {}
};

}

#endif
