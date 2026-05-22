#ifndef THORSANVIL_SLACK_SLACKSTREAM_H
#define THORSANVIL_SLACK_SLACKSTREAM_H

#include "ThorsSlackConfig.h"
#include "NisseHTTP/ClientStream.h"

namespace ThorsAnvil::Slack
{

class Stream: public ThorsAnvil::Nisse::HTTP::ClientStream
{
    public:
        Stream()
            : ThorsAnvil::Nisse::HTTP::ClientStream("slack.com")
        {}
};

}

#endif
