#ifndef THORSANVIL_SLACK_UTIL_H
#define THORSANVIL_SLACK_UTIL_H

#include "ThorsSlackConfig.h"
#include <string>
#include <memory>
#include "BlockKit.h"

namespace ThorsAnvil::Slack
{
        struct InputValueToString   // Extract a string from a unique ptr (use empty string for null)
        {
            inline static std::string const emptyString{};
            std::string operator()(std::unique_ptr<std::string> const& ptr)             {if (!ptr) {return emptyString;}return *ptr;}
            std::string operator()(std::unique_ptr<std::time_t> const& ptr)             {if (!ptr) {return emptyString;}return std::to_string(*ptr);}
            std::string operator()(std::unique_ptr<BlockKit::ElOption> const& ptr)      {if (!ptr) {return emptyString;}return ptr->value;}
            std::string operator()(std::unique_ptr<BlockKit::VecElOption> const& ptr)
            {
                if (!ptr) {return emptyString;}

                // Extract the currently selected options into a set.
                std::string     currentState;
                std::string     seporator = "";     // initial empty;
                for (auto const& option: (*ptr)) {
                    currentState += seporator;
                    currentState += option.value;
                    seporator = ":";
                }

                return currentState;
            }
        };
}

#endif
