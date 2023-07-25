#ifndef THORSANVIL_THORSSOCKET_CONNECTION_SECURE_SOCKET_CONFIG_H
#define THORSANVIL_THORSSOCKET_CONNECTION_SECURE_SOCKET_CONFIG_H

#include "ThorsSocketConfig.h"
#include "ThorsLogging/ThorsLogging.h"

#include <string>
#include <utility>
#include <vector>
#include <functional>

#include <openssl/ssl.h>
#include <openssl/err.h>


namespace ThorsAnvil::ThorsSocket
{
    namespace ConnectionType
    {
        class SSocketBase;
    }

extern "C" int certificateInfo_PasswdCB(char* buf, int size, int /*rwflag*/, void* userdata);
int certificateInfo_PasswdCBNormal(char* buf, int size, int rwflag, void* userdata);
std::string buildOpenSSLErrorMessage(bool prefix = true);
enum class SSLMethodType {Client, Server};

enum Protocol { TLS_1_0, TLS_1_1, TLS_1_2, TLS_1_3 };

class SSLUtil
{
    SSLUtil();
    public:
        static SSLUtil& getInstance();

        SSLUtil(SSLUtil const&)                 = delete;
        SSLUtil& operator=(SSLUtil const&)      = delete;
};

struct ProtocolInfo
{
    private:
        Protocol    minProtocol     = TLS_1_2;
        Protocol    maxProtocol     = TLS_1_3;

        int convertProtocolToOpenSSL(Protocol protocol) const;
    public:
        ProtocolInfo()
            : ProtocolInfo(TLS_1_2, TLS_1_3)
        {}
        ProtocolInfo(Protocol minProtocol, Protocol maxProtocol)
            : minProtocol(minProtocol)
            , maxProtocol(maxProtocol)
        {}

        void apply(SSL_CTX* ctx)   const;
        void apply(SSL* ssl)       const;
};

struct CipherInfo
{
    std::string         cipherList          =   "ECDHE-ECDSA-AES128-GCM-SHA256"     ":"
                                                "ECDHE-RSA-AES128-GCM-SHA256"       ":"
                                                "ECDHE-ECDSA-AES256-GCM-SHA384"     ":"
                                                "ECDHE-RSA-AES256-GCM-SHA384"       ":"
                                                "ECDHE-ECDSA-CHACHA20-POLY1305"     ":"
                                                "ECDHE-RSA-CHACHA20-POLY1305"       ":"
                                                "DHE-RSA-AES128-GCM-SHA256"         ":"
                                                "DHE-RSA-AES256-GCM-SHA384";
    std::string         cipherSuite         =   "TLS_AES_256_GCM_SHA384"            ":"
                                                "TLS_CHACHA20_POLY1305_SHA256"      ":"
                                                "TLS_AES_128_GCM_SHA256";
    void apply(SSL_CTX* ctx)    const;
    void apply(SSL* ssl)        const;
};

class SSLctx
{
    private:
        friend class ConnectionType::SSocketBase;
        SSL_CTX*            ctx;
    public:
        template<typename... Args>
        SSLctx(SSLMethodType methodType, Args&&... args);
               // ProtocolInfo
               // CipherInfo
               // CertificateInfo
               // CertifcateAuthorityInfo
               // ClientCAListInfo

        ~SSLctx();

        SSLctx(SSLctx const&)                   = delete;
        SSLctx& operator=(SSLctx const&)        = delete;

        SSLctx(SSLctx&& move)
            : ctx(std::exchange(move.ctx, nullptr))
        {}
        //SSLctx& operator=(SSLctx const&)        = delete;
    private:
        SSL_METHOD const*       createClient();
        SSL_METHOD const*       createServer();
        SSL_CTX*                newCtx(SSL_METHOD const* method);
};

struct CertificateInfo
{
    public:
        using GetPasswordFunc = std::function<std::string(int)>;
    private:
        friend int ThorsAnvil::ThorsSocket::certificateInfo_PasswdCBNormal(char*, int, int, void*);

        std::string     certificateFileName;
        std::string     keyFileName;
        GetPasswordFunc getPassword;

    public:
        CertificateInfo();
        CertificateInfo(std::string const& certificateFileName, std::string const& keyFileName, GetPasswordFunc&& getPassword = [](int){return "";});

        void apply(SSL_CTX* ctx)   const;
        void apply(SSL* ssl)       const;
};

enum AuthorityType { File, Dir, Store };

template<AuthorityType A>
struct CertifcateAuthorityDataInfo
{
    bool                        loadDefault = false;
    std::vector<std::string>    items;

    void apply(SSL_CTX* ctx)   const;
    int setDefaultCertifcateAuthorityInfo(SSL_CTX* ctx) const;
    int setOneCertifcateAuthorityInfo(SSL_CTX* ctx, char const*) const;
    std::string type() const;
};

struct CertifcateAuthorityInfo
{
    CertifcateAuthorityDataInfo<File>   file;
    CertifcateAuthorityDataInfo<Dir>    dir;
    CertifcateAuthorityDataInfo<Store>  store;

    void apply(SSL_CTX* ctx)   const;
};

template<AuthorityType A>
struct ClientCAListDataInfo
{
    std::vector<std::string>        items;

    int addCAToList(STACK_OF(X509_NAME)* certs, char const* item) const;
};

struct ClientCAListInfo
{
    bool                                verifyClientCA = false;
    ClientCAListDataInfo<File>          file;
    ClientCAListDataInfo<Dir>           dir;
    ClientCAListDataInfo<Store>         store;

    STACK_OF(X509_NAME)* buildCAToList()            const;
    void apply(SSL_CTX* ctx)   const;
    void apply(SSL* ssl)       const;
};

template<typename... Args>
SSLctx::SSLctx(SSLMethodType methodType, Args&&... args)
               // ProtocolInfo protocolRange,
               //CipherInfo const& cipherList,
               //CertificateInfo const& certificate,
               //CertifcateAuthorityInfo const& certifcateAuthority,
               //ClientCAListInfo const& clientCAList)
    : ctx(nullptr)
{
    SSLUtil::getInstance();
    SSL_METHOD const*  method;
    if (methodType == SSLMethodType::Client) {
        method = createClient(); // SSLv23_client_method();
    }
    else {
        method = createServer();
    }

    if (method == nullptr)
    {
        ThorsLogAndThrow("ThorsAnvil::THorsSocket::SSLctx",
                         "SSLctx",
                         "TLS_client_method() failed: ", buildOpenSSLErrorMessage());
    }

    ctx = newCtx(method);
    if (ctx == nullptr)
    {
        ThorsLogAndThrow("ThorsAnvil::ThorsSocket::SSLctx",
                         "SSLctx",
                         "SSL_CTX_new() failed: ", buildOpenSSLErrorMessage());
    }

    (args.apply(ctx),...);
    //protocolRange.setProtocolInfo(ctx);
    //cipherList.setCipherInfo(ctx);
    //certificate.setCertificateInfo(ctx);
    //certifcateAuthority.setCertifcateAuthorityInfo(ctx);
    //clientCAList.setCertifcateAuthorityInfo(ctx);
}


}

#if THORS_SOCKET_HEADER_ONLY
#include "SecureSocketUtil.source"
#endif

#endif
