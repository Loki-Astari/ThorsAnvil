#ifndef THORSANVIL_SERIALIZE_JSON_LEXEMES_H
#define THORSANVIL_SERIALIZE_JSON_LEXEMES_H

#include "SerializeConfig.h"
#include "Serialize.h"
#include <string>

namespace ThorsAnvil::Serialize
{

enum YYTokenType
    {
        JSON_STRING     = 258,
        JSON_TRUE       = 261,
        JSON_FALSE      = 262,
        JSON_NULL       = 263,
        JSON_NUMBER     = 264
    };

}

#endif
