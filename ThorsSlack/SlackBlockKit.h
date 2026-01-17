#ifndef THORSANVIL_SLACK_BLOCKKIT_H
#define THORSANVIL_SLACK_BLOCKKIT_H

#if 0
/*
Using the SlackBlockKit webpage is a rueful tool.
https://app.slack.com/block-kit-builder/T095XJHH589#%7B%22blocks%22:%5B%7B%22type%22:%22section%22,%22text%22:%7B%22type%22:%22mrkdwn%22,%22text%22:%22Hey%20there%20%F0%9F%91%8B%20I'm%20TaskBot.%20I'm%20here%20to%20help%20you%20create%20and%20manage%20tasks%20in%20Slack.%5CnThere%20are%20two%20ways%20to%20quickly%20create%20tasks:%22%7D%7D,%7B%22type%22:%22section%22,%22text%22:%7B%22type%22:%22mrkdwn%22,%22text%22:%22*1%EF%B8%8F%E2%83%A3%20Use%20the%20%60/task%60%20command*.%20Type%20%60/task%60%20followed%20by%20a%20short%20description%20of%20your%20tasks%20and%20I'll%20ask%20for%20a%20due%20date%20(if%20applicable).%20Try%20it%20out%20by%20using%20the%20%60/task%60%20command%20in%20this%20channel.%22%7D%7D,%7B%22type%22:%22section%22,%22text%22:%7B%22type%22:%22mrkdwn%22,%22text%22:%22*2%EF%B8%8F%E2%83%A3%20Use%20the%20_Create%20a%20Task_%20action.*%20If%20you%20want%20to%20create%20a%20task%20from%20a%20message,%20select%20%60Create%20a%20Task%60%20in%20a%20message's%20context%20menu.%20Try%20it%20out%20by%20selecting%20the%20_Create%20a%20Task_%20action%20for%20this%20message%20(shown%20below).%22%7D%7D,%7B%22type%22:%22image%22,%22title%22:%7B%22type%22:%22plain_text%22,%22text%22:%22image1%22,%22emoji%22:true%7D,%22image_url%22:%22https://api.slack.com/img/blocks/bkb_template_images/onboardingComplex.jpg%22,%22alt_text%22:%22image1%22%7D,%7B%22type%22:%22section%22,%22text%22:%7B%22type%22:%22mrkdwn%22,%22text%22:%22%E2%9E%95%20To%20start%20tracking%20your%20team's%20tasks,%20*add%20me%20to%20a%20channel*%20and%20I'll%20introduce%20myself.%20I'm%20usually%20added%20to%20a%20team%20or%20project%20channel.%20Type%20%60/invite%20@TaskBot%60%20from%20the%20channel%20or%20pick%20a%20channel%20on%20the%20right.%22%7D,%22accessory%22:%7B%22type%22:%22conversations_select%22,%22placeholder%22:%7B%22type%22:%22plain_text%22,%22text%22:%22Select%20a%20channel...%22,%22emoji%22:true%7D%7D%7D,%7B%22type%22:%22divider%22%7D,%7B%22type%22:%22context%22,%22elements%22:%5B%7B%22type%22:%22mrkdwn%22,%22text%22:%22%F0%9F%91%80%20View%20all%20tasks%20with%20%60/task%20list%60%5Cn%E2%9D%93Get%20help%20at%20any%20time%20with%20%60/task%20help%60%20or%20type%20*help*%20in%20a%20DM%20with%20me%22%7D%5D%7D,%7B%22dispatch_action%22:true,%22type%22:%22input%22,%22element%22:%7B%22type%22:%22plain_text_input%22,%22action_id%22:%22plain_text_input-action%22%7D,%22label%22:%7B%22type%22:%22plain_text%22,%22text%22:%22Label%22,%22emoji%22:true%7D,%22optional%22:false%7D,%7B%22dispatch_action%22:true,%22type%22:%22input%22,%22element%22:%7B%22type%22:%22plain_text_input%22,%22action_id%22:%22plain_text_input-action%22%7D,%22label%22:%7B%22type%22:%22plain_text%22,%22text%22:%22Label%22,%22emoji%22:true%7D,%22optional%22:false%7D,%7B%22dispatch_action%22:true,%22type%22:%22input%22,%22element%22:%7B%22type%22:%22plain_text_input%22,%22action_id%22:%22plain_text_input-action%22%7D,%22label%22:%7B%22type%22:%22plain_text%22,%22text%22:%22Label%22,%22emoji%22:true%7D,%22optional%22:false%7D,%7B%22dispatch_action%22:true,%22type%22:%22input%22,%22element%22:%7B%22type%22:%22plain_text_input%22,%22action_id%22:%22plain_text_input-action%22%7D,%22label%22:%7B%22type%22:%22plain_text%22,%22text%22:%22Label%22,%22emoji%22:true%7D,%22optional%22:false%7D%5D%7D

Note: The text from this page can be imported using ThorsAnvil::Serialize::JsonImporter()

struct Data
{
    Blocks          blocks;
};
ThorsAnvil_MakeTrait(Data, blocks);

Data data;
stream >> ThorsAnvil::Serialize::JsonImporter(data);
*/


/*
- A Simplification of Blocks in these comment.
-
Blocks:                                         =   Vec<Block>
Block:                                          =   One Of
                                                        Actions
                                                        Context
                                                        ContextActions
                                                        Divider
                                                        File
                                                        Header                  => ElTextPlain
                                                        Image
                                                        Input
                                                        Markdown
                                                        RichText
                                                        Section                 => ElText or Vec<ElText>
                                                        Table                   => Vec<Vec<RichText>>
                                                        Video

ElTextPlain:                                    =   Plain

ElText:                                         =   Plain | MarkDwn

RichText:                                       =   Vec<ElRichTextObj>

ElRichTextObj:                                  =   One Of
                                                        RichTextSection         => Vec<RtElement>
                                                        RichTextList            => Vec<RichTextSection>
                                                        RichTextPreformatted    => Vec<RtElement>
                                                        RichTextQuote           => Vec<RtElement>

RtElement:                                      =   One Of
                                                        ElRtBroadcast
                                                        ElRtColor
                                                        ElRtChannel
                                                        ElRtDate
                                                        ElRtEmoji
                                                        ElRtLink
                                                        ElRtText
                                                        ElRtUser
                                                        ElRtUserGroup

Input:                                          =   One Of
                                                        ElActCheckbox
                                                        ElActDatePicker
                                                        ElActDatetimePicker
                                                        ElActEMail
                                                        ElActNumberInput
                                                        ElActPlainTextInput
                                                        ElActRadioButton
                                                        ElActRichTextInput
                                                        ElActSelectMenu
                                                        ElActTimePicker
                                                        ElActURLInput

ContextActions:                                 = Vec<ElFeedbackButton>

ElFeedbackButton:                               = ElButton ElButton

ElButton:                                       = ElText

Context:                                        = Vec<ElImgItem>

ElImgItem:                                      = One Of
                                                        ElImg
                                                        ElTextPlain
                                                        ElTextMarkDown

Actions:                                        = Vec<ElActive>

ElActive:                                       = One Of
                                                        ElActButton
                                                        ElActCheckbox
                                                        ElActDatePicker
                                                        ElActDatetimePicker
                                                        ElActOverflowMenu
                                                        ElActRadioButton
                                                        ElActSelectMenu
                                                        ElActTimePicker
*/
#endif

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"

#include <string>
#include <optional>
#include <variant>
#include <vector>
#include <utility>
#include <initializer_list>
#include <ctime>

namespace ThorsAnvil::Slack::API
{
    struct SlackSelectedCheckBox;
    struct SlackPlainTextInput;
}

namespace ThorsAnvil::Slack::BlockKit
{

using OptString         = std::optional<std::string>;
using OptTime           = std::optional<std::time_t>;
using OptInt            = std::optional<int>;
using OptBool           = std::optional<bool>;
using VecString         = std::vector<std::string>;
using OptVecString      = std::optional<VecString>;

template<typename T>
using OptVector     = std::optional<std::vector<T>>;

// Section 1:
// Objects used by Blocks.
// See Blocks Below.

    enum TextType { /*vera-ignore*/ plain_text, mrkdwn};
    struct ElTextPlain
    {
        // https://docs.slack.dev/reference/block-kit/composition-objects/text-object
        // TextType                    type = TextType::mrkdwn;           // Can be one of "plain_text" or "mrkdwn"
        std::string                 text;           // The text for the block. This field accepts any of the standard text formatting markup when type is mrkdwn. The minimum length is 1 and maximum length is 3000 characters.
        OptBool                     emoji;          // Indicates whether emojis in a text field should be escaped into the colon emoji format. This field is only usable when type is plain_text.
        OptBool                     verbatim;       // When set to false (as is default) URLs will be auto-converted into links, conversation names will be link-ified, and certain mentions will be automatically parsed.
                                                    // When set to true, Slack will continue to process all markdown formatting and manual parsing strings, but it won’t modify any plain-text content. For example, channel names will not be hyperlinked.
                                                    // This field is only usable when type is mrkdwn.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElTextPlain, plain_text);
        ThorsAnvil_TypeFieldName(type);
    };
    using OptElTextPlain = std::optional<ElTextPlain>;
    struct ElTextMarkDown
    {
        // https://docs.slack.dev/reference/block-kit/composition-objects/text-object
        //TextType                    type = TextType::mrkdwn;           // Can be one of "plain_text" or "mrkdwn"
        std::string                 text;           // The text for the block. This field accepts any of the standard text formatting markup when type is mrkdwn. The minimum length is 1 and maximum length is 3000 characters.
        OptBool                     emoji;          // Indicates whether emojis in a text field should be escaped into the colon emoji format. This field is only usable when type is plain_text.
        OptBool                     verbatim;       // When set to false (as is default) URLs will be auto-converted into links, conversation names will be link-ified, and certain mentions will be automatically parsed.
                                                    // When set to true, Slack will continue to process all markdown formatting and manual parsing strings, but it won’t modify any plain-text content. For example, channel names will not be hyperlinked.
                                                    // This field is only usable when type is mrkdwn.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElTextMarkDown, mrkdwn);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElText
    {
        // https://docs.slack.dev/reference/block-kit/composition-objects/text-object
        TextType                    type; // = TextType::mrkdwn;           // Can be one of "plain_text" or "mrkdwn"
        std::string                 text;           // The text for the block. This field accepts any of the standard text formatting markup when type is mrkdwn. The minimum length is 1 and maximum length is 3000 characters.
        OptBool                     emoji;          // Indicates whether emojis in a text field should be escaped into the colon emoji format. This field is only usable when type is plain_text.
        OptBool                     verbatim;       // When set to false (as is default) URLs will be auto-converted into links, conversation names will be link-ified, and certain mentions will be automatically parsed.
                                                    // When set to true, Slack will continue to process all markdown formatting and manual parsing strings, but it won’t modify any plain-text content. For example, channel names will not be hyperlinked.
                                                    // This field is only usable when type is mrkdwn.
    };
    using OptElText     = std::optional<ElText>;
    using OptElTexts    = std::optional<std::vector<ElText>>;

    struct ElConfirm
    {
        // https://docs.slack.dev/reference/block-kit/composition-objects/confirmation-dialog-object
        ElTextPlain                 title;          // A plain_text text object that defines the dialog's title. Maximum length for this field is 100 characters.
        ElTextPlain                 text;           // A plain_text text object that defines the explanatory text that appears in the confirm dialog. Maximum length for the text in this field is 300 characters.
        ElTextPlain                 confirm;        // A plain_text text object to define the text of the button that confirms the action. Maximum length for the text in this field is 30 characters.
        ElTextPlain                 deny;           // A plain_text text object to define the text of the button that cancels the action. Maximum length for the text in this field is 30 characters.
        OptString                   style;          // Defines the color scheme applied to the confirm button. A value of danger will display the button with a red background on desktop, or red text on mobile. A value of primary will display the button with a green background on desktop, or blue text on mobile. If this field is not provided, the default value will be primary.
    };
    using OptElConfirm = std::optional<ElConfirm>;
    struct ElOption
    {
        // https://docs.slack.dev/reference/block-kit/composition-objects/option-object
        ElText                      text;           // A text object that defines the text shown in the option on the menu. Overflow, select, and multi-select menus can only use plain_text objects, while radio buttons and checkboxes can use mrkdwn text objects. Maximum length for the text in this field is 75 characters.
        std::string                 value;          // A unique string value that will be passed to your app when this option is chosen. Maximum length for this field is 150 characters.
        OptElTextPlain              description;    // A plain_text text object that defines a line of descriptive text shown below the text field beside a single selectable item in a select menu, multi-select menu, checkbox group, radio button group, or overflow menu. Checkbox group and radio button group items can also use mrkdwn formatting. Maximum length for the text within this field is 75 characters.
        OptString                   url;            // A URL to load in the user's browser when the option is clicked. The url attribute is only available in overflow menus. Maximum length for this field is 3000 characters. If you're using url, you'll still receive an interaction payload and will need to send an acknowledgement response.
    };

    using OptElOption = std::optional<ElOption>;
    using VecElOption = std::vector<ElOption>;
    using OptVecElOption = std::optional<VecElOption>;
    struct ElDispatch
    {
        // https://docs.slack.dev/reference/block-kit/composition-objects/dispatch-action-configuration-object
        OptVecString                trigger_actions_on; // An array of interaction types that you would like to receive a block_actions payload for. Should be one or both of:on_enter_pressed — payload is dispatched when user presses the enter key while the input is in focus. Hint text will appear underneath the input explaining to the user to press enter to submit.on_character_entered — payload is dispatched when a character is entered (or removed) in the input.
    };
    using OptElDispatch = std::optional<ElDispatch>;
    struct ElButton
    {
        ElText                      text;           // A text object that defines the button's text. Can only be of type: plain_text. Maximum length for the text in this field is 75 characters.
        std::string                 value;          // The value to send along with the interaction payload. Maximum length is 2000 characters.
        OptString                   accessibility_label; // A label for longer descriptive text about a button element. This label will be read out by screen readers instead of the button text object. Maximum length is 75 characters.
    };
    struct ElActButton
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/button-element/
        // std::string                 type;           // always "button"
        ElText                      text;           // A text object that defines the button's text. Can only be of type: plain_text. text may truncate with ~30 characters. Maximum length for the text in this field is 75 characters
        OptString                   action_id;      // An identifier for this action. You can use this when you receive an interaction payload to identify the source of the action. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        OptString                   url;            // A URL to load in the user's browser when the button is clicked. Maximum length is 3000 characters. If you're using url, you'll still receive an interaction payload and will need to send an acknowledgement response.
        OptString                   value;          // The value to send along with the interaction payload. Maximum length is 2000 characters.
        OptString                   style;          // Decorates buttons with alternative visual color schemes. Use this option with restraint.primary gives buttons a green outline and text, ideal for affirmation or confirmation actions. primary should only be used for one button within a set.danger gives buttons a red outline and text, and should be used when the action is destructive. Use danger even more sparingly than primary.If you don't include this field, the default button style will be used.
        OptElConfirm                confirm;        // A confirm object that defines an optional confirmation dialog after the button is clicked.
        OptString                   accessibility_label; // A label for longer descriptive text about a button element. This label will be read out by screen readers instead of the button text object. Maximum length is 75 characters.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActButton, button);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElActCheckbox
    {
        using ValueStorageType = API::SlackSelectedCheckBox;
        using ValueReturnType  = OptVecElOption;
        // https://docs.slack.dev/reference/block-kit/block-elements/checkboxes-element/
        //std::string                 type;           // always "checkboxes".
        OptString                   action_id;      // An identifier for the action triggered when the checkbox group is changed. You can use this when you receive an interaction payload to identify the source of the action. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        VecElOption                 options;        // An array of option objects. A maximum of 10 options are allowed.
        OptVecElOption              initial_options;// An array of option objects that exactly matches one or more of the options within options. These options will be selected when the checkbox group initially loads.
        OptElConfirm                confirm;        // A confirm object that defines an optional confirmation dialog that appears after clicking one of the checkboxes in this element.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActCheckbox, checkboxes);
        ThorsAnvil_TypeFieldName(type);

        ElActCheckbox& addOption(ElOption val)                           {options.emplace_back(std::move(val));return *this;}
        ElActCheckbox& addOption(std::initializer_list<ElOption> val)    {options.insert(std::end(options), std::move(val));return *this;}
    };
    struct ElActDatePicker
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/date-picker-element/
        // std::string                 type;           // always "datepicker".
        OptString                   action_id;      // An identifier for the action triggered when a menu option is selected. You can use this when you receive an interaction payload to identify the source of the action. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        OptString                   initial_date;   // The initial date that is selected when the element is loaded. This should be in the format YYYY-MM-DD.
        OptElConfirm                confirm;        // A confirm object that defines an optional confirmation dialog that appears after a date is selected.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
        OptElTextPlain              placeholder;    // A plain_text only text object that defines the placeholder text shown on the datepicker. Maximum length for the text in this field is 150 characters.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActDatePicker, datepicker);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElActDatetimePicker
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/datetime-picker-element/
        //std::string                 type;           // always "datetimepicker"
        OptString                   action_id;      // An identifier for the input value when the parent modal is submitted. You can use this when you receive a view_submission payload to identify the value of the input element. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        OptTime                     initial_date_time; // The initial date and time that is selected when the element is loaded, represented as a UNIX timestamp in seconds. This should be in the format of 10 digits, for example 1628633820 represents the date and time August 10th, 2021 at 03:17pm PST.
        OptElConfirm                confirm;        // A confirm object that defines an optional confirmation dialog that appears after a time is selected.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActDatetimePicker, datetimepicker);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElActOverflowMenu
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/overflow-menu-element/
        //std::string                 type;           // always "overflow"
        OptString                   action_id;      // An identifier for the action triggered when a menu option is selected. You can use this when you receive an interaction payload to identify the source of the action. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        VecElOption                 options;        // An array of up to five option objects to display in the menu.
        OptElConfirm                confirm;        // A confirm object that defines an optional confirmation dialog that appears after a menu item is selected.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActOverflowMenu, overflow);
        ThorsAnvil_TypeFieldName(type);

        ElActOverflowMenu& addOption(ElOption val)                           {options.emplace_back(std::move(val));return *this;}
        ElActOverflowMenu& addOption(std::initializer_list<ElOption> val)    {options.insert(std::end(options), std::move(val));return *this;}
    };
    struct ElActRadioButton
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/radio-button-group-element/
        // std::string                 type;           // always "radio_buttons".
        OptString                   action_id;      // An identifier for the action triggered when the radio button group is changed. You can use this when you receive an interaction payload to identify the source of the action. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        VecElOption                 options;        // An array of option objects. A maximum of 10 options are allowed.
        OptElOption                 initial_option; // An option object that exactly matches one of the options within options. This option will be selected when the radio button group initially loads.
        OptElConfirm                confirm;        // A confirm object that defines an optional confirmation dialog that appears after clicking one of the radio buttons in this element.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActRadioButton, radio_buttons);
        ThorsAnvil_TypeFieldName(type);

        ElActRadioButton& addOption(ElOption val)                           {options.emplace_back(std::move(val));return *this;}
        ElActRadioButton& addOption(std::initializer_list<ElOption> val)    {options.insert(std::end(options), std::move(val));return *this;}
    };
    struct ElActSelectMenu
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/select-menu-element/
        //std::string                 type;           // always "static_select"
        OptString                   action_id;      // An identifier for the action triggered when a menu option is selected. You can use this when you receive an interaction payload to identify the source of the action. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        VecElOption                 options;        // An array of option objects. Maximum number of options is 100. If option_groups is specified, this field should not be.
        OptVecElOption              option_groups;  // An array of option group objects. Maximum number of option groups is 100. If options is specified, this field should not be.
        OptElOption                 initial_option; // A single option that exactly matches one of the options within options or option_groups. This option will be selected when the menu initially loads.
        OptElConfirm                confirm;        // A confirm object that defines an optional confirmation dialog that appears after a menu item is selected.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
        OptElTextPlain              placeholder;    // A plain_text only text object that defines the placeholder text shown on the menu. Maximum length for the text in this field is 150 characters.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActSelectMenu, static_select);
        ThorsAnvil_TypeFieldName(type);

        ElActSelectMenu& addOption(ElOption val)                           {options.emplace_back(std::move(val));return *this;}
        ElActSelectMenu& addOption(std::initializer_list<ElOption> val)    {options.insert(std::end(options), std::move(val));return *this;}
    };
    struct ElActTimePicker
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/time-picker-element/
        //std::string                 type;           // always "timepicker"
        OptString                   action_id;      // An identifier for the action triggered when a time is selected. You can use this when you receive an interaction payload to identify the source of the action. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        OptString                   initial_time;   // The initial time that is selected when the element is loaded. This should be in the format HH:mm, where HH is the 24-hour format of an hour (00 to 23) and mm is minutes with leading zeros (00 to 59), for example 22:25 for 10:25pm.
        OptElConfirm                confirm;        // A confirm object that defines an optional confirmation dialog that appears after a time is selected.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
        OptElTextPlain              placeholder;    // A plain_text only text object that defines the placeholder text shown on the time picker. Maximum length for the text in this field is 150 characters.
        OptString                   timezone;       // A string in the IANA format, e.g. "America/Chicago". The timezone is displayed to end users as hint text underneath the time picker. It is also passed to the app upon certain interactions, such as view_submission.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActTimePicker, timepicker);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElActEMail
    {
        //https://docs.slack.dev/reference/block-kit/block-elements/email-input-element/
        //std::string                 type;           // always "email_text_input"
        OptString                   action_id;      // An identifier for the input value when the parent modal is submitted. You can use this when you receive a view_submission payload to identify the value of the input element. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        OptString                   initial_value;  // The initial value in the email input when it is loaded.
        OptElDispatch               dispatch_action_config; // A dispatch configuration object that determines when during text input the element returns a block_actions payload.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
        OptElTextPlain              placeholder;    // A plain_text only text object that defines the placeholder text shown in the email input. Maximum length for the text in this field is 150 characters.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActEMail, email_text_input);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElActNumberInput
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/number-input-element/
        //std::string                 type;           // always "number_input"
        bool                        is_decimal_allowed; // Decimal numbers are allowed if is_decimal_allowed= true, set the value to false otherwise.
        OptString                   action_id;      // An identifier for the input value when the parent modal is submitted. You can use this when you receive a view_submission payload to identify the value of the input element. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        OptString                   initial_value;  // The initial value in the plain-text input when it is loaded.
        OptString                   min_value;      // The minimum value, cannot be greater than max_value.
        OptString                   max_value;      // The maximum value, cannot be less than min_value.
        OptElDispatch               dispatch_action_config; // A dispatch configuration object that determines when during text input the element returns a block_actions payload.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
        OptElTextPlain              placeholder;    // A plain_text only text object that defines the placeholder text shown in the number input. Maximum length for the text in this field is 150 characters.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActNumberInput, number_input);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElActPlainTextInput
    {
        using ValueStorageType = API::SlackPlainTextInput;
        using ValueReturnType  = std::string;
        // https://docs.slack.dev/reference/block-kit/block-elements/plain-text-input-element/
        // std::string                 type;           // always "plain_text_input"
        OptString                   action_id;      // An identifier for the input value when the parent modal is submitted. You can use this when you receive a view_submission payload to identify the value of the input element. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        OptString                   initial_value;  // The initial value in the plain-text input when it is loaded.
        OptBool                     multiline;      // Indicates whether the input will be a single line (false) or a larger textarea (true). Defaults to false.
        OptInt                      min_length;     // The minimum length of input that the user must provide. If the user provides less, they will receive an error. Acceptable values for this field are between 0 and 3000, inclusive.
        OptInt                      max_length;     // The maximum length of input that the user can provide. If the user provides more, they will receive an error. Acceptable values for this field are between 1 and 3000, inclusive.
        OptElDispatch               dispatch_action_config; // A dispatch configuration object that determines when during text input the element returns a block_actions payload.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
        OptElTextPlain              placeholder;    // A plain_text only text object that defines the placeholder text shown in the plain-text input. Maximum length for the text in this field is 150 characters.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActPlainTextInput, plain_text_input);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElActURLInput
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/url-input-element/
        // std::string                 type;           // always "url_text_input"
        OptString                   action_id;      // An identifier for the input value when the parent modal is submitted. You can use this when you receive a view_submission payload to identify the value of the input element. Should be unique among all other action_ids in the containing block. Maximum length is 255 characters.
        OptString                   initial_value;  // The initial value in the URL input when it is loaded.
        OptElDispatch               dispatch_action_config; // A dispatch configuration object that determines when during text input the element returns a block_actions payload.
        OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
        OptElTextPlain              placeholder;    // A plain_text only text object that defines the placeholder text shown in the URL input. Maximum length for the text in this field is 150 characters.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActURLInput, url_text_input);
        ThorsAnvil_TypeFieldName(type);
    };
    using ElActive  = std::variant<ElActButton, ElActCheckbox, ElActDatePicker, ElActDatetimePicker, ElActOverflowMenu, ElActRadioButton, ElActSelectMenu, ElActTimePicker>;
    using OptElActive = std::optional<ElActive>;

    struct ElImageFile
    {
        // https://docs.slack.dev/reference/block-kit/composition-objects/slack-file-object
        OptString                   url;            // This URL can be the url_private or the permalink of the Slack file.
        OptString                   id;             // Slack ID of the file.
    };
    using OptElImageFile = std::optional<ElImageFile>;
    struct ElImg
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/image-element
        // std::string                 type;           // always "image"
        std::string                 alt_text;       // A plain-text summary of the image. This should not contain any markup.
        OptString                   image_url;      // The URL for a publicly hosted image. You must provide either an image_url or slack_file. Maximum length for this field is 3000 characters.
        OptElImageFile              slack_file;     // A Slack image file object that defines the source of the image.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElImg, image);
        ThorsAnvil_TypeFieldName(type);
    };
    using ElImgItem = std::variant<ElImg, ElTextPlain, ElTextMarkDown>;

    struct ElFeedbackButton
    {
        // https://docs.slack.dev/reference/block-kit/block-elements/feedback-buttons-element
        //std::string                 type;           // always "feedback_buttons"
        ElButton                    positive_button;// A button to indicate positive feedback. See button object fields below.
        ElButton                    negative_button;// A button to indicate negative feedback. See button object fields below.
        OptString                   action_id;      // An identifier for this action. You can use this when you receive an interaction payload to identify the source of the action. Should be unique among all other action_id values in the containing block. Maximum length is 255 characters.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElFeedbackButton, feedback_buttons);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElSlackFile
    {
        // https://docs.slack.dev/reference/block-kit/composition-objects/slack-file-object
        OptString                   url;            // This URL can be the url_private or the permalink of the Slack file.
        OptString                   id;             // Slack ID of the file.
    };
    using OptElSlackFile  = std::optional<ElSlackFile>;

    struct InfoStyle
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        OptBool     bold;
        OptBool     italic;
        OptBool     strike;
        OptBool     highlight;
        OptBool     client_highlight;
        OptBool     unlink;
    };
    struct InfoText
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        bool        bold;
        bool        italic;
        bool        strike;
        bool        code;
    };
    using OptInfoStyle = std::optional<InfoStyle>;
    using OptInfoText = std::optional<InfoText>;

    enum BroadcastType {/*vera-ignore*/here, channel, everyone};
    struct ElRtBroadcast
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        BroadcastType       range;      // The range of the broadcast; value can be here, channel, or everyone.
                                        // Using here notifies only the active members of a channel; channel notifies all members of a channel;
                                        // everyone notifies every person in the #general channel.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElRtBroadcast, broadcast);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElRtColor
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        // This provides a color block!
        std::string         value;      // The hex value for the color.
                                        // Must be proceeded by # => #FF0000 => Red
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElRtColor, color);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElRtChannel
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        std::string         channel_id; // The ID of the channel to be mentioned.
        OptInfoStyle        style;      // An object of six optional boolean properties that dictate style: bold, italic, strike, highlight, client_highlight, and unlink.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElRtChannel, channel);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElRtDate
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        std::time_t         timestamp;  // A Unix timestamp for the date to be displayed in seconds.
        std::string         format;     // A template string containing curly-brace-enclosed tokens to substitute your provided timestamp. See details below.
        OptString           url;        // URL to link the entire format string to.
        OptString           fallback;   // Text to display in place of the date should parsing, formatting or displaying fail.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElRtDate, date);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElRtEmoji
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        std::string         name;       // The name of the emoji; i.e. "wave" or "wave::skin-tone-2".
        OptString           unicode;    // Represents the unicode code point of the emoji, where applicable.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElRtEmoji, emoji);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElRtLink
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        std::string         url;        // The link's url.
        OptString           text;       // The text shown to the user (instead of the url). If no text is provided, the url is used.
        OptBool             unsafe;     // Indicates whether the link is safe.
        OptInfoText         style;      // An object containing four boolean properties: bold, italic, strike, and code.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElRtLink, link);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElRtText
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        std::string         text;       // The text shown to the user.
        OptInfoText         style;      // An object containing four boolean fields, none of which are required: bold, italic, strike, and code.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElRtText, text);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElRtUser
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        std::string         user_id;    // The ID of the user to be mentioned.
        OptInfoStyle        style;      // An object of six optional boolean properties that dictate style: bold, italic, strike, highlight, client_highlight, and unlink.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElRtUser, user);
        ThorsAnvil_TypeFieldName(type);
    };
    struct ElRtUserGroup
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        std::string         usergroup_id;   // The ID of the user group to be mentioned.
        OptInfoStyle        style;          // An object of six optional boolean properties that dictate style: bold, italic, strike, highlight, client_highlight, and unlink.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElRtUserGroup, usergroup);
        ThorsAnvil_TypeFieldName(type);
    };
    using RtElement = std::variant<ElRtBroadcast, ElRtColor, ElRtChannel, ElRtDate, ElRtEmoji, ElRtLink, ElRtText, ElRtUser, ElRtUserGroup>;
    using VecRtElement = std::vector<RtElement>;
    struct RichTextSection
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        VecRtElement            elements;
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::RichTextSection, rich_text_section);
        ThorsAnvil_TypeFieldName(type);
    };
    using VecRichTextSection = std::vector<RichTextSection>;
    enum ListType {/*vera-ignore*/bullet, ordered};
    struct RichTextList
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        ListType                style;      // Either "bullet" or "ordered"
        VecRichTextSection      elements;   // An array of rich_text_section objects containing two properties: type, which is "rich_text_section", and elements, which is an array of rich text element objects.
        OptInt                  indent;     // Number of pixels to indent the list.
        OptInt                  offset;     // Number to offset the first number in the list. For example, if the offset = 4, the first number in the ordered list would be 5.
        OptInt                  border;     // Number of pixels of border thickness.
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::RichTextList, rich_text_list);
        ThorsAnvil_TypeFieldName(type);
    };
    struct RichTextPreformatted
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        VecRtElement            elements;   // An array of rich text elements.
        OptInt                  border;     // Number of pixels of border thickness. (1 or 0)
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::RichTextPreformatted, rich_text_preformatted);
        ThorsAnvil_TypeFieldName(type);
    };
    struct RichTextQuote
    {
        // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block#element-types
        VecRtElement            elements;   // An array of rich text elements.
        OptInt                  border;     // Number of pixels of border thickness. (1 or 0)
        ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::RichTextQuote, rich_text_quote);
        ThorsAnvil_TypeFieldName(type);
    };
    using ElRichTextObj = std::variant<RichTextSection, RichTextList, RichTextPreformatted, RichTextQuote>;
    using VecElRichTextObj = std::vector<ElRichTextObj>;


    struct ElColInfo
    {
        // https://docs.slack.dev/reference/block-kit/blocks/table-block/
        OptString               align;          // The alignment for items in this column. Can be left, center, or right. Defaults to left if not defined.
        OptBool                 is_wrapped;     // Whether the contents of this column should be wrapped or not. Defaults to false if not defined.
    };
    using VecElColInfo = std::vector<ElColInfo>;
    using OptVecElColInfo = std::optional<VecElColInfo>;

// Section 2:
// Different Types of Blocks
struct Actions
{
    // https://docs.slack.dev/reference/block-kit/blocks/actions-block/
    //std::string                 type;           // always "actions"
    std::vector<ElActive>       elements;       // Max 25 elements.
                                                // An array of interactive element objects - buttons, select menus, overflow menus, or date pickers.
    OptString                   block_id;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Actions, actions);
    ThorsAnvil_TypeFieldName(type);
};
struct Context
{
    // https://docs.slack.dev/reference/block-kit/blocks/context-block/
    //std::string                 type;           // always "context"
    std::vector<ElImgItem>      elements;       // Max 10 elements
                                                // An array of image elements and text objects.
    OptString                   block_id;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Context, context);
    ThorsAnvil_TypeFieldName(type);
};
struct ContextActions
{
    // https://docs.slack.dev/reference/block-kit/blocks/context-actions-block/
    //std::string                 type;           // always "context_actions"
    std::vector<ElFeedbackButton> elements;       // Max 5 elements.
                                                // An array of feedback buttons elements and icon button elements.
    OptString                   block_id;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ContextActions, context_actions);
    ThorsAnvil_TypeFieldName(type);
};
struct Divider
{
    // https://docs.slack.dev/reference/block-kit/blocks/divider-block/
    //std::string                 type;           // always "divider"
    OptString                   block_id;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Divider, divider);
    ThorsAnvil_TypeFieldName(type);
};
// You can not send this message block.
// But it can be received from SLACK.
//     Currently not testable.
struct File
{
    // https://docs.slack.dev/reference/block-kit/blocks/file-block/
    //std::string                 type;           // always "file"
    std::string                 external_id;    // The external unique ID for this file.
    std::string                 source;         // At the moment, source will always be remote for a remote file.
    OptString                   block_id;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::File, file);
    ThorsAnvil_TypeFieldName(type);
};
struct Header
{
    // https://docs.slack.dev/reference/block-kit/blocks/header-block/
    //std::string                 type;           // always "header"
    ElTextPlain                 text;           // The text for the block, in the form of a plain_text text object. Maximum length for the text in this field is 150 characters.
    OptString                   block_id;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Header, header);
    ThorsAnvil_TypeFieldName(type);
};
struct Image
{
    // https://docs.slack.dev/reference/block-kit/blocks/image-block/
    //std::string                 type;           // always "image"
    std::string                 alt_text;       // A plain-text summary of the image. This should not contain any markup. Maximum length for this field is 2000 characters.
    OptString                   image_url;      // The URL for a publicly hosted image. You must provide either an image_url or slack_file. Maximum length for this field is 3000 characters.
    OptElSlackFile              slack_file;     // A Slack image file object that defines the source of the image.
    OptElText                   title;          // An optional title for the image in the form of a text object that can only be of type: plain_text. Maximum length for the text in this field is 2000 characters.
    OptString                   block_id;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Image, image);
    ThorsAnvil_TypeFieldName(type);
};
struct Markdown
{
    // https://docs.slack.dev/reference/block-kit/blocks/markdown-block/
    //std::string                 type;           // always "markdown"
    std::string                 text;           // The standard markdown-formatted text. Limit 12,000 characters max.
    OptString                   block_id;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Markdown, markdown);
    ThorsAnvil_TypeFieldName(type);
};
struct RichText
{
    // https://docs.slack.dev/reference/block-kit/blocks/rich-text-block/
    //std::string                 type;           // always "rich_text"
    VecElRichTextObj            elements;       // An array of rich text objects - rich_text_section, rich_text_list, rich_text_preformatted, and rich_text_quote. See your specific desired element below for more details.
    OptString                   block_id;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::RichText, rich_text);
    ThorsAnvil_TypeFieldName(type);

    RichText& addSection(std::initializer_list<std::string> val);
    RichText& addSection(std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val);
    RichText& addList(ListType type, std::initializer_list<std::string> val);
    RichText& addList(ListType type, std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val);
    RichText& addPreForm(int border, std::initializer_list<std::string> val);
    RichText& addPreForm(int border, std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val);
    RichText& addPreForm(std::initializer_list<std::string> val);
    RichText& addPreForm(std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val);
    RichText& addQuote(int border, std::initializer_list<std::string> val);
    RichText& addQuote(int border, std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val);
    RichText& addQuote(std::initializer_list<std::string> val);
    RichText& addQuote(std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val);
};
using OptRichText = std::optional<RichText>;
/* OUT OF ODER:
 * Needs the Rich Text Element
 */
struct ElActRichTextInput
{
    // std::string                 type;           // always "rich_text_input"
    std::string                 action_id;      // An identifier for the input value when the parent modal is submitted. You can use this when you receive a view_submission payload to identify the value of the input element. Should be unique in the containing block. Maximum length is 255 characters.
    OptRichText                 initial_value;  // The initial value in the rich text input when it is loaded.
    OptElDispatch               dispatch_action_config; // A dispatch configuration object that determines when during text input the element returns a block_actions payload.
    OptBool                     focus_on_load;  // Indicates whether the element will be set to auto focus within the view object. Only one element can be set to true. Defaults to false.
    OptElTextPlain              placeholder;    // A plain_text object that defines the placeholder text shown in the plain-text input. Maximum length for the text in this field is 150 characters.
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActRichTextInput, rich_text_input);
    ThorsAnvil_TypeFieldName(type);
};
struct Section
{
    // https://docs.slack.dev/reference/block-kit/blocks/section-block/
    //std::string                 type;           // always "section"
    // Preferred
    OptElText                   text;           // The text for the block, in the form of a text object. Minimum length for the text in this field is 1 and maximum length is 3000 characters.
                                                // This field is not required if a valid array of fields objects is provided instead.
    // Maybe: Alternative to text
    OptElTexts                  fields;         // Required if no text is provided. An array of text objects. Any text objects included with fields will be rendered in a compact format that allows for 2 columns of side-by-side text.
                                                // Maximum number of items is 10. Maximum length for the text in each item is 2000 characters.
    OptString                   block_id;
    OptElActive                 accessory;      // One of the compatible element objects noted above. Be sure to confirm the desired element works with section.
    OptBool                     expand;         // Whether or not this section block's text should always expand when rendered.
                                                // If false or not provided, it may be rendered with a 'see more' option to expand and show the full text.
                                                // For AI Assistant apps, this allows the app to post long messages without users needing to click 'see more' to expand the message.
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Section, section);
    ThorsAnvil_TypeFieldName(type);
};
using ElCell = RichText;
using ElRow = std::vector<ElCell>;
using ElTable = std::vector<ElRow>;
struct Table
{
    // https://docs.slack.dev/reference/block-kit/blocks/table-block/
    //std::string                 type;           // Always "table".
    OptString                   block_id;
    ElTable                     rows;           // An array consisting of table rows.
                                                // Maximum 100 rows.
                                                // Each row object is an array with a max of 20 table cells. Table cells can have a type of raw_text or rich_text.
    OptVecElColInfo             column_settings;// An array describing column behavior.
                                                // If there are fewer items in the column_settings array than there are columns in the table, then the items in the the column_settings array will
                                                // describe the same number of columns in the table as there are in the array itself. Any additional columns will have the default behavior.
                                                // Maximum 20 items. See below for column settings schema.
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Table, table);
    ThorsAnvil_TypeFieldName(type);
};
struct Video
{
    // https://docs.slack.dev/reference/block-kit/blocks/video-block/
    //std::string                 type;           // always "video"
    std::string                 alt_text;       // A tooltip for the video. Required for accessibility
    OptString                   author_name;    // Author name to be displayed. Must be less than 50 characters.
    OptString                   block_id;
    OptElTextPlain              description;    // Description for video in the form of a text object that must have type of plain_text. text within must be less than 200 characters.
    OptString                   provider_icon_url; // Icon for the video provider, e.g. YouTube icon.
    OptString                   provider_name;  // The originating application or domain of the video, e.g. YouTube.
    ElTextPlain                 title;          // Video title in the form of a text object that must have type of plain_text. text within must be less than 200 characters.
    OptString                   title_url;      // Hyperlink for the title text. Must correspond to the non-embeddable URL for the video. Must go to an HTTPS URL.
    std::string                 thumbnail_url;  // The thumbnail image URL
    std::string                 video_url;      // The URL to be embedded. Must match any existing unfurl domains within the app and point to a HTTPS URL.
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Video, video);
    ThorsAnvil_TypeFieldName(type);
};
using InputElement = std::variant<ElActCheckbox, ElActDatePicker, ElActDatetimePicker, ElActEMail, ElActNumberInput, ElActPlainTextInput, ElActRadioButton/*, ElActRichTextInput*/, ElActSelectMenu, ElActTimePicker, ElActURLInput>;
struct Input
{
    // https://docs.slack.dev/reference/block-kit/blocks/input-block/
    //std::string                 type;           // always "input".
    ElText                      label;          // A label that appears above an input element in the form of a text object that must have type of plain_text. Maximum length for the text in this field is 2000 characters.
    InputElement                element;        // A block element. See above for full list.
    OptBool                     dispatch_action;// A boolean that indicates whether or not the use of elements in this block should dispatch a block_actions payload.
                                                // Defaults to false.
                                                // This field is incompatible with the file_input block element. If dispatch_action is set to true and a file_input block element is provided, an unsupported type error will be raised.
    OptString                   block_id;
    OptElText                   hint;           // An optional hint that appears below an input element in a lighter grey. It must be a text object with a type of plain_text. Maximum length for the text in this field is 2000 characters.
    OptBool                     optional;       // A boolean that indicates whether the input element may be empty when a user submits the modal.
                                                // Defaults to false.
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::Input, input);
    ThorsAnvil_TypeFieldName(type);
};

using Block = std::variant<Actions, Context, ContextActions, Divider, File, Header, Image, Input, Markdown, RichText, Section, Table, Video>;
using Blocks = std::vector<Block>;
using OptBlocks = std::optional<Blocks>;


inline ElOption makeOptionPlain(std::string text, std::string value)                            {return ElOption{.text=ElText{.type=plain_text, .text=std::move(text)}, .value=std::move(value)};}
inline ElOption makeOptionPlain(std::string text, std::string value, std::string description)   {return ElOption{.text=ElText{.type=plain_text, .text=std::move(text)},.value=std::move(value),.description=ElTextPlain{.text=std::move(description)}};}
inline ElOption makeOptionMarkD(std::string text, std::string value)                            {return ElOption{.text=ElText{.type=mrkdwn, .text=std::move(text)}, .value=std::move(value)};}
inline ElOption makeOptionMarkD(std::string text, std::string value, std::string description)   {return ElOption{.text=ElText{.type=mrkdwn, .text=std::move(text)},.value=std::move(value),.description=ElTextPlain{.text=std::move(description)}};}
inline
InfoText makeInfoText(std::initializer_list<bool> const& init)
{
    InfoText    result;
    if (std::size(init) > 0) {
        result.bold = std::data(init)[0];
    }
    if (std::size(init) > 1) {
        result.italic = std::data(init)[1];
    }
    if (std::size(init) > 2) {
        result.strike = std::data(init)[2];
    }
    if (std::size(init) > 3) {
        result.code = std::data(init)[3];
    }
    return result;
}
inline
RichTextSection makeRichTextSection(std::initializer_list<std::string> elements)
{
    RichTextSection result;
    for (auto& val: elements) {
        result.elements.emplace_back(ElRtText{.text=std::move(val)});
    };
    return result;
}
inline
RichTextSection makeRichTextSection(std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> elements)
{
    RichTextSection result;
    for (auto& val: elements) {
        result.elements.emplace_back(
            ElRtText{
                .text=std::move(val.first),
                .style=makeInfoText(val.second)
            }
        );
    };
    return result;
}
inline
RichTextList makeRichTextList(ListType type, std::initializer_list<std::string> elements)
{
    RichTextList result{.style = type};
    for (auto& val: elements) {
        result.elements.emplace_back(makeRichTextSection({std::move(val)}));
    };
    return result;
}
inline
RichTextList makeRichTextList(ListType type, std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> elements)
{
    RichTextList result{.style = type};
    for (auto& val: elements) {
        result.elements.emplace_back(makeRichTextSection({std::move(val)}));
    };
    return result;
}
inline
RichTextPreformatted makeRichTextPreForm(std::initializer_list<std::string> elements)
{
    RichTextPreformatted result;
    for (auto& val: elements) {
        result.elements.emplace_back(ElRtText{.text = std::move(val)});
    };
    return result;
}
inline
RichTextPreformatted makeRichTextPreForm(std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> elements)
{
    RichTextPreformatted result;
    for (auto& val: elements) {
        result.elements.emplace_back(ElRtText{.text = std::move(val.first), .style = makeInfoText(val.second)});
    };
    return result;
}
inline
RichTextPreformatted makeRichTextPreForm(int border, std::initializer_list<std::string> elements)
{
    RichTextPreformatted result{.border = border};
    for (auto& val: elements) {
        result.elements.emplace_back(ElRtText{.text = std::move(val)});
    };
    return result;
}
inline
RichTextPreformatted makeRichTextPreForm(int border, std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> elements)
{
    RichTextPreformatted result{.border = border};
    for (auto& val: elements) {
        result.elements.emplace_back(ElRtText{.text = std::move(val.first), .style = makeInfoText(val.second)});
    };
    return result;
}
inline
RichTextQuote makeRichTextQuote(std::initializer_list<std::string> elements)
{
    RichTextQuote result;
    for (auto& val: elements) {
        result.elements.emplace_back(ElRtText{.text = std::move(val)});
    };
    return result;
}
inline
RichTextQuote makeRichTextQuote(std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> elements)
{
    RichTextQuote result;
    for (auto& val: elements) {
        result.elements.emplace_back(ElRtText{.text = std::move(val.first), .style = makeInfoText(val.second)});
    };
    return result;
}
inline
RichTextQuote makeRichTextQuote(int border, std::initializer_list<std::string> elements)
{
    RichTextQuote result{.border = border};
    for (auto& val: elements) {
        result.elements.emplace_back(ElRtText{.text = std::move(val)});
    };
    return result;
}
inline
RichTextQuote makeRichTextQuote(int border, std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> elements)
{
    RichTextQuote result{.border = border};
    for (auto& val: elements) {
        result.elements.emplace_back(ElRtText{.text = std::move(val.first), .style = makeInfoText(val.second)});
    };
    return result;
}
inline RichText& RichText::addSection(std::initializer_list<std::string> val)                                                          {elements.emplace_back(makeRichTextSection(std::move(val)));return *this;}
inline RichText& RichText::addSection(std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val)                  {elements.emplace_back(makeRichTextSection(std::move(val)));return *this;}
inline RichText& RichText::addList(ListType type, std::initializer_list<std::string> val)                                              {elements.emplace_back(makeRichTextList(type, std::move(val)));return *this;}
inline RichText& RichText::addList(ListType type, std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val)      {elements.emplace_back(makeRichTextList(type, std::move(val)));return *this;}
inline RichText& RichText::addPreForm(int border, std::initializer_list<std::string> val)                                              {elements.emplace_back(makeRichTextPreForm(border, std::move(val)));return *this;}
inline RichText& RichText::addPreForm(int border, std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val)      {elements.emplace_back(makeRichTextPreForm(border, std::move(val)));return *this;}
inline RichText& RichText::addPreForm(std::initializer_list<std::string> val)                                                          {elements.emplace_back(makeRichTextPreForm(std::move(val)));return *this;}
inline RichText& RichText::addPreForm(std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val)                  {elements.emplace_back(makeRichTextPreForm(std::move(val)));return *this;}
inline RichText& RichText::addQuote(int border, std::initializer_list<std::string> val)                                                {elements.emplace_back(makeRichTextQuote(border, std::move(val)));return *this;}
inline RichText& RichText::addQuote(int border, std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val)        {elements.emplace_back(makeRichTextQuote(border, std::move(val)));return *this;}
inline RichText& RichText::addQuote(std::initializer_list<std::string> val)                                                            {elements.emplace_back(makeRichTextQuote(std::move(val)));return *this;}
inline RichText& RichText::addQuote(std::initializer_list<std::pair<std::string, std::initializer_list<bool>>> val)                    {elements.emplace_back(makeRichTextQuote(std::move(val)));return *this;}
template<typename T> inline
Section makeSectionPlain(T text)    {return Section{.text = ElText{.type=TextType::plain_text,.text=std::move(text)}};}
template<typename T> inline
Section makeSectionMarkD(T text)    {return Section{.text = ElText{.type=TextType::mrkdwn,.text=std::move(text)}};}

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElTextPlain, text, emoji, verbatim);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElTextMarkDown, text, emoji, verbatim);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElText, type, text, emoji, verbatim);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElConfirm, title, text, confirm, deny, style);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElOption, text, value, description, url);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElDispatch, trigger_actions_on);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElButton, text, value, accessibility_label);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElImageFile, url, id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElFeedbackButton, positive_button, negative_button, action_id);

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActButton, text, action_id, url, value, style, confirm, accessibility_label);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActCheckbox, action_id, options, initial_options, confirm, focus_on_load);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActDatePicker, action_id, initial_date, confirm, focus_on_load, placeholder);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActDatetimePicker, action_id, initial_date_time, confirm, focus_on_load);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActOverflowMenu, action_id, options, confirm);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActRadioButton, action_id, options, initial_option, confirm, focus_on_load);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActSelectMenu, action_id, options, option_groups, initial_option, confirm, focus_on_load, placeholder);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActTimePicker, action_id, initial_time, confirm, focus_on_load, placeholder, timezone);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActEMail, action_id, initial_value, dispatch_action_config, focus_on_load, placeholder);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActNumberInput, is_decimal_allowed, action_id, initial_value, min_value, max_value, dispatch_action_config, focus_on_load, placeholder);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActPlainTextInput, action_id, initial_value, multiline, min_length, max_length, dispatch_action_config, focus_on_load, placeholder);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActRichTextInput, action_id, initial_value, dispatch_action_config, focus_on_load, placeholder);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElActURLInput, action_id, initial_value, dispatch_action_config, focus_on_load, placeholder);

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElImg, alt_text, image_url, slack_file);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElSlackFile, url, id);
// -- Rich Text
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::InfoStyle, bold, italic, strike, highlight, client_highlight, unlink);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::InfoText, bold, italic, strike, code);
// ---
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElRtBroadcast, range);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElRtColor, value);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElRtChannel, channel_id, style);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElRtDate, timestamp, format, url, fallback);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElRtEmoji, name, unicode);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElRtLink, url, text, unsafe, style);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElRtText, text, style);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElRtUser, user_id, style);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElRtUserGroup, usergroup_id, style);
//---
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::RichTextSection, elements);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::RichTextList, style, elements, indent, offset, border);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::RichTextPreformatted, elements, border);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::RichTextQuote, elements, border);
// -- Rich Text
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ElColInfo, align, is_wrapped);
/// ----
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Actions, elements, block_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Context, elements, block_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::ContextActions, elements, block_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Divider, block_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::File, external_id, source, block_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Header, text, block_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Image, alt_text, image_url, slack_file, title, block_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Input, label, element, dispatch_action, block_id, hint, optional);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Markdown, text, block_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::RichText, elements, block_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Section, text, fields, block_id, accessory, expand);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Table, block_id, rows, column_settings);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::BlockKit::Video, alt_text, author_name, block_id, description, provider_icon_url, provider_name, title, title_url, thumbnail_url, video_url);


#endif
