#ifndef THORSANVIL_THORSSOCKET_CONNECTION_SECURE_SOCKET_H
#define THORSANVIL_THORSSOCKET_CONNECTION_SECURE_SOCKET_H

#include "ThorsSocketConfig.h"
#include "ConnectionSocket.h"

#include <cstddef>

namespace ThorsAnvil::ThorsSocket::ConnectionType
{

class SSocket;

enum class DeferAction  {None, Connect, Accept};

class SSocketStandard
{
    SSL*        ssl;
    bool        connectionFailed;
    DeferAction deferAction;
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

        void   deferInit(YieldFunc& rYield, YieldFunc& wYield);

    private:
        void initSSocket(SSLctx const& ctx, int fd);
        void initSSocketClientConnect(YieldFunc& rYield, YieldFunc& wYield);
        void initSSocketClientAccept(YieldFunc& rYield, YieldFunc& wYield);
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
        virtual std::string_view protocol() const override {return "https";}

        virtual IOData readFromStream(char* buffer, std::size_t size)       override;
        virtual IOData writeToStream(char const* buffer, std::size_t size)  override;
        virtual void   deferInit(YieldFunc& rYield, YieldFunc& wYield) override {secureSocketInfo.deferInit(rYield, wYield);}
};

class SSocketServer: public SocketServer
{
    SSLctx          ctx;

    public:
        SSocketServer(SServerInfo&& socketInfo, Blocking blocking);

        virtual std::unique_ptr<ConnectionClient> accept(YieldFunc& yield, Blocking blocking, DeferAccept deferAccept)          override;
};

}

#if THORS_SOCKET_HEADER_ONLY
#include "ConnectionSSocket.source"
#endif

#endif
