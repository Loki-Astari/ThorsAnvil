#ifndef THORSANVIL_SLACK_SLACKCLIENT_H
#define THORSANVIL_SLACK_SLACKCLIENT_H

#include "ThorsSlackConfig.h"
#include "API.h"
#include "APIAuth.h"
#include "SlackStream.h"
#include "NisseHTTP/ClientRequest.h"
#include "NisseHTTP/ClientResponse.h"
#include "NisseHTTP/HeaderResponse.h"
#include "NisseHTTP/StreamInput.h"
#include "NisseHTTP/Util.h"
#include "ThorSerialize/ParserConfig.h"
#include "ThorSerialize/PrinterConfig.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"

#include <string>
#include <string_view>
#include <iostream>
#include <functional>
#include <utility>
#include <variant>

namespace ThorsAnvil::Slack
{

using namespace std::literals::string_literals;
namespace Ser   = ThorsAnvil::Serialize;
namespace Nisse = ThorsAnvil::Nisse::HTTP;

template<typename T>
using SuccFunc = std::function<void(T&&)>;
using FailFunc = std::function<void(API::Error&&)>;

template<typename T>
struct VisitResult
{
    SuccFunc<T>&& succ;
    FailFunc&&    fail;
    VisitResult(SuccFunc<T>&& succ, FailFunc&& fail)
        : succ(std::move(succ))
        , fail(std::move(fail))
    {}

    void operator()(T& val)            {succ(std::move(val));}
    void operator()(API::Error& val)   {fail(std::move(val));}
};

class SlackClient
{
    Nisse::HeaderResponse   botHeaders;
    Nisse::HeaderResponse   userHeaders;
    std::string             botId;
    private:
        template<typename T>
        void sendMessageData(T const& message, SlackStream& stream)
        {
            if constexpr (T::method == API::Method::GET) {
                std::string api = std::string{} + T::api + "?" + ThorsAnvil::Slack::API::buildQueryA(message);
                Nisse::ClientRequest    post(stream, api, T::method);
                if constexpr (T::scope == API::Scope::Bot) {
                    post.addHeaders(botHeaders);
                }
                else {
                    post.addHeaders(userHeaders);
                }
                post.body(0);
            }
            else {
                // Anything that is not a GET
                Nisse::ClientRequest    post(stream, T::api, T::method);
                if constexpr (T::scope == API::Scope::Bot) {
                    post.addHeaders(botHeaders);
                }
                else {
                    post.addHeaders(userHeaders);
                }
                std::size_t size = Ser::jsonStreanSize(message);
                post.body(size) << Ser::jsonExporter(message, Ser::PrinterConfig{Ser::OutputType::Stream});
            }
        }
        template<typename T>
        std::string getEventType(Nisse::StreamInput& input, bool& hit)
        {
            if (hit) {
                return "";
            }
            hit = true;
            std::string_view    body = input.preloadStreamIntoBuffer();
            if (body.find(R"("ok":false)") != std::string_view::npos) {
                ThorsLogInfo("ThorsAnvil::Slack::SlackClient", "getEventType", "Found: Error");
                return "ThorsAnvil::Slack::API::Error";
            }
            if (body.find(R"("ok":true)") != std::string_view::npos) {
                ThorsLogInfo("ThorsAnvil::Slack::SlackClient", "getEventType", "Found: Result: ", T::polyMorphicSerializerName());
                return T::polyMorphicSerializerName();
            }
            ThorsLogTrack("ThorsAnvil::Slack::SlackClient", "getEventType", "Found: Fallback object members");
            return "";
        }
    public:
        SlackClient(std::string const& botToken, std::string const& userToken)
        {
            botHeaders.add("Connection", "close");
            botHeaders.add("Content-Type", "application/json; charset=utf-8");
            botHeaders.add("Authorization", "Bearer " + botToken);

            userHeaders.add("Connection", "close");
            userHeaders.add("Content-Type", "application/json; charset=utf-8");
            userHeaders.add("Authorization", "Bearer " + userToken);

            sendMessage(ThorsAnvil::Slack::API::Auth::Test{},
                        [&](ThorsAnvil::Slack::API::Auth::Test::Reply&& result)
                        {
                            botId = result.user_id;
                        });
        }
        std::string const& getBotId()   {return botId;}

        template<typename T>
        void  sendMessage(T const& message, SuccFunc<typename T::Reply>&& succ = [](typename T::Reply&&){}, FailFunc&& fail = [](API::Error&&){})
        {
            using ResultType = typename T::Reply;
            using OutputType = std::variant<API::Error, ResultType>;

            SlackStream             stream;
            sendMessageData(message, stream);

            Nisse::ClientResponse   response(stream);
            Nisse::StreamInput      input(stream, response.getContentSize());
            OutputType              reply;
            bool hit = false;
            input >> Ser::jsonImporter(reply, Ser::ParserConfig{}.setIdentifyDynamicClass([&](Ser::DataInputStream&){return getEventType<ResultType>(input, hit);}));

            std::visit(VisitResult<ResultType>{std::move(succ), std::move(fail)}, reply);
        }
        /*
         * This is used to check the SLACK response object to validate that it matches the documentation.
         * You should not be using this unless you are doing some debugging or setting up a new response type
         */
        template<typename T>
        void  validateMessage(T const& message)
        {
            SlackStream             stream;
            std::cerr << "Sending: " << ThorsAnvil::Serialize::jsonExporter(message) << "\n----------\n";
            sendMessageData(message, stream);

            Nisse::ClientResponse   response(stream);
            Nisse::StreamInput      input(stream, response.getContentSize());
            std::string line;
            while (std::getline(input, line)) {
                std::cerr << "L: " << line << "\n";
            }
            std::cerr << "DONE\n\n";
        }
        template<typename T>
        bool  sendMessage(T const& message, typename T::Reply& result, bool dumpError = false)
        {
            bool good = true;
            sendMessage(message, [&result](typename T::Reply&& value){result = std::move(value);}, [&dumpError,&good](API::Error&& value){good = false;if (dumpError){std::cerr << ThorsAnvil::Serialize::jsonExporter(value) << "\n";}});
            return good;
        }
};

}

#endif
