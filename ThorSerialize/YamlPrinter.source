#include "SerializeConfig.h"
#include "YamlPrinter.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <sstream>

extern "C"
{
    int thorsanvilYamlStreamWritter(void* data, unsigned char* buffer, size_t size);
}

using namespace ThorsAnvil::Serialize;

HEADER_ONLY_INCLUDE
int thorsanvilYamlStreamWritter(void* data, unsigned char* buffer, size_t size)
{
    YamlPrinter*     owner = reinterpret_cast<YamlPrinter*>(data);
    owner->output.write(reinterpret_cast<char*>(buffer), size);
    bool result      = static_cast<bool>(owner->output);

    return result ? 1 : 0;
}

HEADER_ONLY_INCLUDE
void YamlPrinter::checkYamlResultCode(int code, char const* method, char const* msg)
{
    static std::string const  msgBase   = "ThorsAnvil::Serialize::YamlPrinter::";

    if (code == 0)
    {
        error = true;
        ThorsLogAndThrow("ThorsAnvil::Serialize::YamlPrinter",
                         "checkYamlResultCode",
                         method, ":", msg);
    }
}
HEADER_ONLY_INCLUDE
void YamlPrinter::checkYamlResultCode(std::function<int(yaml_event_t&)> init, char const* method, char const* msg)
{
    yaml_event_t    event;
    int             code1   = init(event);
    checkYamlResultCode(code1, method, msg);
    int             code2   = yaml_emitter_emit(&emitter, &event);
    checkYamlResultCode(code2, method, "yaml_emitter_emit");
}

HEADER_ONLY_INCLUDE
YamlPrinter::YamlPrinter(std::ostream& output, PrinterConfig config)
    : PrinterInterface(output, config)
    , error(false)
{
    checkYamlResultCode(yaml_emitter_initialize(&emitter), "YamlPrinter", "yaml_emitter_initialize");
    yaml_emitter_set_output(&emitter, thorsanvilYamlStreamWritter, this);
    checkYamlResultCode(
            [&](yaml_event_t& event){return yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);},
            "YamlPrinter",
            "yaml_stream_start_event_initialize");
    state.emplace_back(0, TraitType::Value);
}
HEADER_ONLY_INCLUDE
YamlPrinter::~YamlPrinter()
{
    if (!error)
    {
        checkYamlResultCode(
            [&](yaml_event_t& event){return yaml_stream_end_event_initialize(&event);},
            "~YamlPrinte",
            "yaml_stream_end_event_initialize");
    }
    checkYamlResultCode(yaml_emitter_flush(&emitter), "~YamlPrinter", "yaml_emitter_flush");
    yaml_emitter_delete(&emitter);
}

HEADER_ONLY_INCLUDE
void YamlPrinter::openDoc()
{
    checkYamlResultCode(
            [&](yaml_event_t& event){return yaml_document_start_event_initialize(&event, NULL, NULL, NULL, 0);},
            "openDoc",
            "yaml_document_start_event_initialize");
}
HEADER_ONLY_INCLUDE
void YamlPrinter::closeDoc()
{
    checkYamlResultCode(
            [&](yaml_event_t& event){return yaml_document_end_event_initialize(&event, 0);},
            "closeDoc",
            "yaml_document_end_event_initialize");
    checkYamlResultCode(yaml_emitter_flush(&emitter), "closeDoc", "yaml_emitter_flush");
}

HEADER_ONLY_INCLUDE
void YamlPrinter::openMap(std::size_t)
{
    yaml_mapping_style_t    style;
    switch (this->config.characteristics)
    {
        case PrinterInterface::OutputType::Stream:  style   = YAML_FLOW_MAPPING_STYLE;  break;
        case PrinterInterface::OutputType::Config:  style   = YAML_BLOCK_MAPPING_STYLE; break;
        default:                                    style   = YAML_ANY_MAPPING_STYLE;   break;
    }
    checkYamlResultCode(
            [&](yaml_event_t& event){return yaml_mapping_start_event_initialize(&event, NULL, NULL, 1, style);},
            "openMap",
            "yaml_mapping_start_event_initialize");
    state.emplace_back(0, TraitType::Map);
}
HEADER_ONLY_INCLUDE
void YamlPrinter::closeMap()
{
    checkYamlResultCode(
            [&](yaml_event_t& event){return yaml_mapping_end_event_initialize(&event);},
            "closeMap",
            "yaml_mapping_end_event_initialize");
    state.pop_back();
}
HEADER_ONLY_INCLUDE
void YamlPrinter::openArray(std::size_t)
{
    yaml_sequence_style_t    style;
    switch (this->config.characteristics)
    {
        case PrinterInterface::OutputType::Stream:  style   = YAML_FLOW_SEQUENCE_STYLE; break;
        case PrinterInterface::OutputType::Config:  style   = YAML_BLOCK_SEQUENCE_STYLE;break;
        default:                                    style   = YAML_ANY_SEQUENCE_STYLE;  break;
    }
    checkYamlResultCode(
            [&](yaml_event_t& event){return yaml_sequence_start_event_initialize(&event, NULL, NULL, 1, style);},
            "openArray",
            "yaml_sequence_start_event_initialize");
    state.emplace_back(0, TraitType::Array);
}
HEADER_ONLY_INCLUDE
void YamlPrinter::closeArray()
{
    checkYamlResultCode(
            [&](yaml_event_t& event){return yaml_sequence_end_event_initialize(&event);},
            "closeArray",
            "yaml_sequence_end_event_initialize");
    state.pop_back();
}

template<typename T>
void YamlPrinter::emit(T const& data)
{
    std::stringstream buffer;
    buffer << data;

    checkYamlResultCode(
        [&](yaml_event_t& event)
        {
            return yaml_scalar_event_initialize(
                        &event,
                        NULL,
                        NULL,
                        reinterpret_cast<yaml_char_t*>(const_cast<char*>(buffer.str().c_str())),
                        buffer.str().size(),
                        1,
                        0,
                        YAML_ANY_SCALAR_STYLE);
        },
        "emit",
        "yaml_scalar_event_initialize");
    ++state.back().first;
}
HEADER_ONLY_INCLUDE
void YamlPrinter::emitNull()
{
    static yaml_char_t nullObject[] = "null";
    checkYamlResultCode(
        [&](yaml_event_t& event)
        {
            return yaml_scalar_event_initialize(
                        &event,
                        NULL,
                        NULL,
                        nullObject,
                        4,
                        1,
                        1,
                        YAML_ANY_SCALAR_STYLE);
        },
        "emit",
        "yaml_scalar_event_initialize");
    ++state.back().first;
}

HEADER_ONLY_INCLUDE
void YamlPrinter::addKey(std::string const& key)
{
    if (state.back().second != TraitType::Map && state.back().first % 2 != 1)
    {
        error = true;
        ThorsLogAndThrow("ThorsAnvil::Serialize::YamlPrinter",
                         "addKey",
                         "Invalid call to addKey(): Currently not in a map");
    }
    emit(key);
}
