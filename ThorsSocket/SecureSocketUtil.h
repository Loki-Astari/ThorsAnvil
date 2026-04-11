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
        class SSocketStandard;
    }

extern "C" int certificateInfo_PasswdCB(char* buf, int size, int /*rwflag*/, void* userdata);
int certificateInfo_PasswdCBNormal(char* buf, int size, int rwflag, void* userdata);
std::string buildOpenSSLErrorMessage(bool prefix = true);
enum class  SSLMethodType   { Client, Server};

enum Protocol               { TLS_1_0, TLS_1_1, TLS_1_2, TLS_1_3};
enum AuthorityType          { File, Dir, Store};
enum SystemDefault          { Load};
enum VerifyPeer             { On};
enum MarkUsed : int         { ProtocolMark = 0, CipherMark = 1, CertificateMark = 2, AuthorityFileMark = 3, AuthorityDirMark = 4, AuthorityStoreMark = 5, ClientMark = 6, END = 7};

using StringList    = std::vector<std::string>;
using MarkArray     = std::array<bool, MarkUsed::END>;

class SSLUtil
{
    SSLUtil();
    public:
        static SSLUtil& getInstance();

        SSLUtil(SSLUtil const&)                 = delete;
        SSLUtil& operator=(SSLUtil const&)      = delete;
};

/*
 * SSL manipulators
 *   Objects of these type can be passed to SSLctx constructor.
 *   These can be passed as const reference values and thus constructed inline if needed.
 *
 *      ProtocolInfo
 *      CipherInfo
 *      CertificateInfo
 *      CertifcateAuthorityFile
 *      CertifcateAuthorityDir
 *      CertifcateAuthorityStore
 *      ClientCAListInfo
 */


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

        void apply(SSL_CTX* ctx, MarkArray& mark)   const;
        void apply(SSL* ssl)       const;

        static constexpr MarkUsed markType = MarkUsed::ProtocolMark;
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
    void apply(SSL_CTX* ctx, MarkArray& mark)    const;
    void apply(SSL* ssl)        const;

    static constexpr MarkUsed markType = MarkUsed::CipherMark;
};

struct CertificateInfo
{
    public:
        using GetPasswordFunc = std::function<std::string(int)>;
    private:
        friend int ThorsAnvil::ThorsSocket::certificateInfo_PasswdCBNormal(char*, int, int, void*);

        std::string     certificateFileName;
        std::string     keyFileName;
        bool            hasPasswordGetter;
        GetPasswordFunc getPassword;

    public:
        CertificateInfo(std::string const& certificateFileName, std::string const& keyFileName);
        CertificateInfo(std::string const& certificateFileName, std::string const& keyFileName, GetPasswordFunc&& getPassword);

        void apply(SSL_CTX* ctx, MarkArray& mark)   const;
        void apply(SSL* ssl)       const;

        static constexpr MarkUsed markType = MarkUsed::CertificateMark;
};

static constexpr MarkUsed certifcateAuthorityDataInfoType[] = {AuthorityFileMark, AuthorityDirMark, AuthorityStoreMark};

template<AuthorityType A>
class CertifcateAuthorityDataInfo
{
    bool                        loadDefault;
    StringList                  items;

    int setDefaultCertifcateAuthorityInfo(SSL_CTX* ctx) const;
    int setOneCertifcateAuthorityInfo(SSL_CTX* ctx, char const*) const;
    std::string type() const;

    public:
        CertifcateAuthorityDataInfo(SystemDefault, StringList fileList = {})
            : loadDefault(true)
            , items(std::move(fileList))
        {}
        CertifcateAuthorityDataInfo(SystemDefault, std::string file)
            : loadDefault(true)
            , items({std::move(file)})
        {}
        CertifcateAuthorityDataInfo(StringList fileList)
            : loadDefault(false)
            , items(std::move(fileList))
        {}
        CertifcateAuthorityDataInfo(std::string file)
            : loadDefault(false)
            , items({std::move(file)})
        {}

        void apply(SSL_CTX* ctx, MarkArray& mark)   const;

        static constexpr MarkUsed markType = certifcateAuthorityDataInfoType[A];
};

using CertifcateAuthorityFile   = CertifcateAuthorityDataInfo<File>;
using CertifcateAuthorityDir    = CertifcateAuthorityDataInfo<Dir>;
using CertifcateAuthorityStore  = CertifcateAuthorityDataInfo<Store>;

template<AuthorityType A>
struct ClientCAListDataInfo
{
    StringList      items;

    int addCAToList(STACK_OF(X509_NAME)* certs, char const* item) const;
};

class ClientCAListInfo
{
    bool                                verify;
    ClientCAListDataInfo<File>          file;
    ClientCAListDataInfo<Dir>           dir;
    ClientCAListDataInfo<Store>         store;

    STACK_OF(X509_NAME)* buildCAToList()            const;

    public:
        ClientCAListInfo():             verify(false)   {}
        ClientCAListInfo(VerifyPeer):   verify(true)    {}
        ClientCAListInfo& addFile(std::string f)        {file.items.emplace_back(std::move(f));return *this;}
        ClientCAListInfo& addFiles(StringList fl)       {std::move(std::begin(fl), std::end(fl), std::back_inserter(file.items));return *this;}
        ClientCAListInfo& addDir(std::string f)         {dir.items.emplace_back(std::move(f));return *this;}
        ClientCAListInfo& addDirs(StringList fl)        {std::move(std::begin(fl), std::end(fl), std::back_inserter(dir.items));return *this;}
        ClientCAListInfo& addStore(std::string f)       {store.items.emplace_back(std::move(f));return *this;}
        ClientCAListInfo& addStores(StringList fl)      {std::move(std::begin(fl), std::end(fl), std::back_inserter(store.items));return *this;}

        void apply(SSL_CTX* ctx, MarkArray& mark)   const;
        void apply(SSL* ssl)       const;

        static constexpr MarkUsed markType = MarkUsed::ClientMark;
};

template<typename T>
concept IsMarkType = requires(T a)
{
    // Requires that a.buildHtml() is a valid expression
    // and returns a type convertible to std::string.
    { a.markType } -> std::convertible_to<MarkUsed>;
};


class SSLctx
{
    private:
        friend class ConnectionType::SSocketBase;
        friend class ConnectionType::SSocketStandard;
        SSL_CTX*            ctx;
        MarkArray           mark;
    public:
        template<IsMarkType... Args>
        SSLctx(SSLMethodType methodType, Args&&... args);
            // All the following parameters can be passed in args.
            // They are then applied to the CTX object via the apply() method.
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
        SSLctx& operator=(SSLctx&& move)
        {
            ctx = std::exchange(move.ctx, nullptr);
            return *this;
        }
    private:
        SSL_METHOD const*       createClient();
        SSL_METHOD const*       createServer();
        SSL_CTX*                newCtx(SSL_METHOD const* method);
};

template<IsMarkType... Args>
SSLctx::SSLctx(SSLMethodType methodType, Args&&... args)
    : ctx(nullptr)
    , mark{}
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
        ThorsLogAndThrowDebug(std::runtime_error,
                              "ThorsAnvil::THorsSocket::SSLctx",
                              "SSLctx",
                              "TLS_client_method() failed: ", buildOpenSSLErrorMessage());
    }

    ctx = newCtx(method);
    if (ctx == nullptr)
    {
        ThorsLogAndThrowDebug(std::runtime_error,
                              "ThorsAnvil::ThorsSocket::SSLctx",
                              "SSLctx",
                              "SSL_CTX_new() failed: ", buildOpenSSLErrorMessage());
    }

    (args.apply(ctx, mark),...);

    if (methodType == SSLMethodType::Client) {
        bool check = mark[MarkUsed::AuthorityFileMark] || mark[MarkUsed::AuthorityDirMark] || mark[MarkUsed::AuthorityStoreMark] || mark[MarkUsed::ClientMark];
        if (!check) {
            CertifcateAuthorityFile{SystemDefault::Load}.apply(ctx, mark);
            CertifcateAuthorityDir{SystemDefault::Load}.apply(ctx, mark);
            CertifcateAuthorityStore{SystemDefault::Load}.apply(ctx, mark);
            ClientCAListInfo{VerifyPeer::On}.apply(ctx, mark);
        }
    }
}


}

#if THORS_SOCKET_HEADER_ONLY
#include "SecureSocketUtil.source"
#endif

#endif
