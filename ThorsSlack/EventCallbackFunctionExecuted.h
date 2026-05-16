#ifndef THORSANVIL_SLACK_EVENT_FUNCTION_EXECUTED_H
#define THORSANVIL_SLACK_EVENT_FUNCTION_EXECUTED_H

// Documentation: https://docs.slack.dev/reference/events/function_executed

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <map>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/function_executed
  "event":
  {
    "type": "function_executed",
    "function":
    {
        "id": "Fn123456789O",
        "callback_id": "sample_function",
        "title": "Sample function",
        "description": "Runs sample function",
        "type": "app",
        "input_parameters": [ { "type": "...", "name": "user_id", "description": "...", "title": "User", "is_required": true } ],
        "output_parameters": [ { "type": "...", "name": "user_id", "description": "...", "title": "Greeting", "is_required": true } ],
        "app_id": "AP123456789",
        "date_created": 1694727597,
        "date_updated": 1698947481,
        "date_deleted": 0
    },
    "inputs": { "user_id": "USER12345678" },
    "function_execution_id": "Fx1234567O9L",
    "workflow_execution_id": "WxABC123DEF0",
    "event_ts": "1698958075.998738",
    "bot_access_token": "abcd-..."
  }
#endif

struct FunctionParameter
{
    std::string                 type;
    std::string                 name;
    std::string                 description;
    std::string                 title;
    bool                        is_required;
};
using VecFunctionParameter = std::vector<FunctionParameter>;

struct FunctionDef
{
    std::string                 id;
    std::string                 callback_id;
    std::string                 title;
    std::string                 description;
    std::string                 type;
    VecFunctionParameter        input_parameters;
    VecFunctionParameter        output_parameters;
    std::string                 app_id;
    std::time_t                 date_created;
    std::time_t                 date_updated;
    std::time_t                 date_deleted;
};

using FunctionInputs = std::map<std::string, std::string>;

struct FunctionExecuted
{
    // std::string                 type;
    FunctionDef                 function;
    FunctionInputs              inputs;
    std::string                 function_execution_id;
    std::string                 workflow_execution_id;
    std::string                 event_ts;
    std::string                 bot_access_token;
    static std::string const& typeName() {static std::string const name{"Event/FunctionExecuted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FunctionExecuted, function_executed);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FunctionParameter, type, name, description, title, is_required);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FunctionDef, id, callback_id, title, description, type, input_parameters, output_parameters, app_id, date_created, date_updated, date_deleted);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FunctionExecuted, function, inputs, function_execution_id, workflow_execution_id, event_ts, bot_access_token);

#endif
