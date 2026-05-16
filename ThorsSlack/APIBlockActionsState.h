#ifndef THORSANVIL_SLACK_API_BLOCKACTIONS_STATE_H
#define THORSANVIL_SLACK_API_BLOCKACTIONS_STATE_H

#include "ThorsSlackConfig.h"
#include "API.h"
#include "SlackBlockKit.h"
#include "Util.h"
#include <string>
#include <memory>

namespace BK = ThorsAnvil::Slack::BlockKit;

namespace ThorsAnvil::Slack::API
{
struct SlackUser
{
    std::string                         id;
    std::string                         username;
    std::string                         name;
    std::string                         team_id;
};

struct SlackTeam
{
    std::string                         id;
    std::string                         domain;
};


using NullVecElOption   = std::unique_ptr<BK::VecElOption>;
using NullElText        = std::unique_ptr<BK::ElText>;
using NullElOption      = std::unique_ptr<BK::ElOption>;
using NullString        = std::unique_ptr<std::string>;
using NullTime          = std::unique_ptr<std::time_t>;

// The value for a CheckBox
struct CheckboxesValue
{
    // std::string                         type;               // checkboxes Have Example
    NullVecElOption                     selected_options;
    NullVecElOption const&  getValue() const {return selected_options;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::CheckboxesValue, checkboxes);
    ThorsAnvil_TypeFieldName(type);
};

struct DatePickerValue
{
    // std::string                         type;               // datepicker Have Example
    NullString                          selected_date;
    NullString const&  getValue() const {return selected_date;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::DatePickerValue, datepicker);
    ThorsAnvil_TypeFieldName(type);
};

struct DateTimePickerValue
{
    // std::string                         type;               // datetimepicker Have Example
    NullTime                            selected_date_time;
    NullTime const&  getValue() const {return selected_date_time;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::DateTimePickerValue, datetimepicker);
    ThorsAnvil_TypeFieldName(type);
};

// The value for a RadioButton
struct RadioButtonsValue
{
    // std::string                         type;               // radio_buttons: Have Example
    NullElOption                        selected_option;
    NullElOption const&  getValue() const {return selected_option;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::RadiosButtonValue, radio_buttons);
    ThorsAnvil_TypeFieldName(type);
};

struct StaticSelectValue
{
    // std::string                         type;               // static_select: Have Example
    NullElOption                        selected_option;
    NullElOption const&  getValue() const {return selected_option;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::StaticSelectValue, static_select);
    ThorsAnvil_TypeFieldName(type);
};

struct TimePickerValue
{
    // std::string                         type;               // timepicker: Have Example
    NullString                          selected_time;
    NullString const&  getValue() const {return selected_time;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::TimePickerValue, timepicker);
    ThorsAnvil_TypeFieldName(type);
};

struct EmailTextInputValue
{
    // std::string                         type;               // email_text_input => NEED EXAMPLE <=
    NullString                          initial_value;
    NullString const&  getValue() const {return initial_value;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::EmailTextInputValue, email_text_input);
    ThorsAnvil_TypeFieldName(type);
};

struct NumberInputValue
{
    // std::string                         type;               // number_input => NEED EXAMPLE <=
    NullString                          initial_value;
    NullString const&  getValue() const {return initial_value;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::NumberInputValue, number_input);
    ThorsAnvil_TypeFieldName(type);
};

// The value for a text input field.
struct PlainTextInputValue
{
    //std::string                         type;               // plain_text_input
    NullString                          value;

    NullString const&  getValue() const {return value;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::PlainTextInputValue, plain_text_input);
    ThorsAnvil_TypeFieldName(type);
};

struct URLTextInputValue
{
    // std::string                         type;               // url_text_input: => NEED EXAMPLE <=
    NullString                          initial_value;
    NullString const&  getValue() const {return initial_value;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::URLTextInputValue, url_text_input);
    ThorsAnvil_TypeFieldName(type);
};


// See Also InputElement.
// This set of variants should be kept in sync with the type InputElement.
using InputValue = std::variant<CheckboxesValue, DatePickerValue, DateTimePickerValue, RadioButtonsValue, StaticSelectValue, TimePickerValue, EmailTextInputValue, NumberInputValue, PlainTextInputValue, /*RichTextInputValue,*/ URLTextInputValue>;


// action_id of the "InputElement" object
// Note The "InputElement" is part of the "Input" object.
using SlackValue  = std::map<std::string, InputValue>;

// block_id of the "Input" object
using SlackValues = std::map<std::string, SlackValue>;
struct SlackState
{
    SlackValues                         values;
    template<typename T>
    typename T::ValueReturnType const& getValue(std::string const& block_id, std::string const& action_id) const
    {
        auto findBlock = values.find(block_id);
        if (findBlock != std::end(values)) {
            auto const& actionMap = findBlock->second;
            auto findAction = actionMap.find(action_id);
            if (findAction != std::end(actionMap)) {
                return std::get<typename T::ValueStorageType>(findAction->second).getValue();
            }
        }
        static typename T::ValueReturnType nullOption;
        return nullOption;
    }

    template<typename T>
    std::string getStringValue(std::string const& block_id, std::string const& action_id) const
    {
        InputValueToString   converter;
        return converter(getValue<T>(block_id, action_id));
    };
};
using NullSlackState = std::unique_ptr<SlackState>;

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackUser, id, username, name, team_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackTeam, id, domain);

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::CheckboxesValue, selected_options);         //
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::DatePickerValue, selected_date);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::DateTimePickerValue, selected_date_time);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::EmailTextInputValue, initial_value);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::NumberInputValue, initial_value);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::PlainTextInputValue, value);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::RadioButtonsValue, selected_option);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::StaticSelectValue, selected_option);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::TimePickerValue, selected_time);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::URLTextInputValue, initial_value);

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackState, values);

#endif
