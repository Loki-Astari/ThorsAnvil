#ifndef THORSANVIL_THORSSOCKET_CONNECTION_SECURE_SOCKET_H
#define THORSANVIL_THORSSOCKET_CONNECTION_SECURE_SOCKET_H

#include "ThorsSocketConfig.h"
#include "ConnectionSocket.h"

#include <cstddef>

namespace ThorsAnvil::ThorsSocket::ConnectionType
{

class SSocket;

class SSocketStandard
{
    SSL*        ssl;
    bool        connectionFailed;
    public:
        SSocketStandard()                       = delete;
        SSocketStandard(SSocketStandard const&) = delete;
        SSocketStandard(SSocketStandard&&)      = delete;

        SSocketStandard(SSocketInfo const& socketInfo, int fd);
        SSocketStandard(OpenSSocketInfo const& socketInfo, int fd);
        ~SSocketStandard();

        bool isConnected() const;
        void close();
        void externalyClosed();

        std::string buildSSErrorMessage(int);

        SSL* getSSL() const;
        void checkConnectionOK(int errorCode);

    private:
        void initSSocket(SSLctx const& ctx, int fd);
        void initSSocketClient();
        void initSSocketClientAccept();
};

class SSocketServer;
class SSocketClient: public SocketClient
{
    SSocketStandard     secureSocketInfo;
    public:
        // Normal Client.
        SSocketClient(SSocketInfo const& socketInfo, Blocking blocking);
        // Server Side accept.
        SSocketClient(SSocketServer&, OpenSSocketInfo const& socketInfo, Blocking blocking);
        virtual ~SSocketClient();

        virtual bool isConnected() const override;
        virtual void close()             override;
        virtual void externalyClosed()   override;

        virtual IOData readFromStream(char* buffer, std::size_t size)       override;
        virtual IOData writeToStream(char const* buffer, std::size_t size)  override;
};

class SSocketServer: public SocketServer
{
    SSLctx const&       ctx;

    public:
        SSocketServer(SServerInfo const& socketInfo, Blocking blocking);

        virtual std::unique_ptr<ConnectionClient> accept(Blocking blocking, AcceptFunc&& accept = [](){})          override;
};

}

#if THORS_SOCKET_HEADER_ONLY
#include "ConnectionSSocket.source"
#endif

#endif
