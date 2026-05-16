#ifndef  THORSANVIL_NISSE_BOLT_APP_CONFIG_H
#define  THORSANVIL_NISSE_BOLT_APP_CONFIG_H

#include "NisseBoltConfig.h"
#include "ThorSerialize/Traits.h"

#include <string>

namespace ThorsAnvil::Nisse::Bolt
{

struct AppConfig
{
    std::string     slot;
    std::string     botToken;
    std::string     userToken;
    std::string     signingSecret;
};


}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::Bolt::AppConfig, slot, botToken, userToken, signingSecret);

#endif
