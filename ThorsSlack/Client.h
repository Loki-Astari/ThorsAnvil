#ifndef THORSANVIL_SLACK_SLACKCLIENT_H
#define THORSANVIL_SLACK_SLACKCLIENT_H

#include "ThorsSlackConfig.h"
#include "API.h"
#include "APIAuth.h"
#include "Stream.h"
#include "NisseHTTP/ClientRequest.h"
#include "NisseHTTP/ClientResponse.h"
#include "NisseHTTP/HeaderResponse.h"
#include "NisseHTTP/StreamInput.h"
#include "NisseHTTP/Util.h"
#include "ThorSerialize/ParserConfig.h"
#include "ThorSerialize/PrinterConfig.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Logging.h"

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

class Client
{
    private:
        Nisse::HeaderResponse   botHeaders;
        Nisse::HeaderResponse   userHeaders;
        std::string             botId;

    private:
        template<typename T>
        void sendMessageData(T const& message, Stream& stream) const
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
    public:
        template<typename T>
        static std::string getEventType(Nisse::StreamInput& input)
        {
            std::string_view    body = input.preloadStreamIntoBuffer();
            std::size_t         offset = 0;
            while (true)
            {
                if (body.find(R"("ok":false)", offset) != std::string_view::npos) {
                    ThorsLogInfo("ThorsAnvil::Slack::Client", "getEventType", "Found: Error");
                    return "ThorsAnvil::Slack::API::Error";
                }
                if (body.find(R"("ok":true)", offset) != std::string_view::npos) {
                    ThorsLogInfo("ThorsAnvil::Slack::Client", "getEventType", "Found: Result: ", T::polyMorphicSerializerName());
                    return T::polyMorphicSerializerName();
                }

                // Info not in the buffer. Force more data into the buffer.
                std::size_t oldSize = std::size(body);

                // The -10 is to account for the search string being split across a chunk boundary.
                offset = (oldSize > 10) ? oldSize - 10 : 0;

                // Force the stream to load more data from socket
                body = input.preloadStreamIntoBuffer(true);
                if (std::size(body) == oldSize) {
                    // No more data
                    break;
                }
            }

            ThorsLogTrack("ThorsAnvil::Slack::Client", "getEventType", "Found: Fallback object members");
            return "";
        }

        Client(std::string const& botToken, std::string const& userToken)
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
        std::string const& getBotId()   const {return botId;}

        template<typename T>
        void  sendMessage(T const& message, SuccFunc<typename T::Reply>&& succ = [](typename T::Reply&&){}, FailFunc&& fail = [](API::Error&&){}) const
        {
            using ResultType = typename T::Reply;
            using OutputType = std::variant<API::Error, ResultType>;
            ThorsLogTrackWithData(message, "ThorsAnvil::Slack::Client", "sendMessage", "Sending Request");

            Stream                  stream;
            sendMessageData(message, stream);

            Nisse::ClientResponse   response(stream);
            Nisse::StreamInput      input(stream, response.getContentSize());
            OutputType              reply;
            input >> Ser::jsonImporter(reply, Ser::ParserConfig{}.setIdentifyDynamicClass([&](Ser::DataInputStream&){return getEventType<ResultType>(input);}));
            ThorsLogTrackWithData(reply, "ThorsAnvil::Slack::Client", "sendMessage", "Response:");

            std::visit(VisitResult<ResultType>{std::move(succ), std::move(fail)}, reply);
        }
        /*
         * This is used to check the SLACK response object to validate that it matches the documentation.
         * You should not be using this unless you are doing some debugging or setting up a new response type
         */
        template<typename T>
        void  validateMessage(T const& message) const
        {
            Stream                  stream;
            sendMessageData(message, stream);

            Nisse::ClientResponse   response(stream);
            Nisse::StreamInput      input(stream, response.getContentSize());
            std::string line;
            while (std::getline(input, line)) {
                // Don't remove this function is used for debugging.
                std::cerr << "L: " << line << "\n";
            }
            std::cerr << "DONE\n\n";
        }
        template<typename T>
        bool  sendMessage(T const& message, typename T::Reply& result, bool dumpError = false) const
        {
            bool good = true;
            sendMessage(message, [&result](typename T::Reply&& value){result = std::move(value);}, [&dumpError,&good](API::Error&& value){good = false;if (dumpError){/*DON'T remove*/std::cerr << ThorsAnvil::Serialize::jsonExporter(value) << "\n";}});
            return good;
        }
};

}

#endif
