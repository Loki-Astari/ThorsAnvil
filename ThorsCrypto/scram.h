#ifndef THORSANVIL_CRYPTO_SCRAM_H
#define THORSANVIL_CRYPTO_SCRAM_H

#include "ThorsCryptoConfig.h"
#include "cryptstring.h"
#include "hash.h"
#include "hmac.h"
#include "pbkdf2.h"
#include "base64.h"
#include "ThorsLogging/ThorsLogging.h"
#include <openssl/rand.h>
#include <algorithm>

// RFC-5801 Salted Challenge Response Authentication Mechanism (SCRAM) SASL and GSS-API Mechanisms

namespace ThorsAnvil::Crypto
{

template<typename Hi, typename HMAC, typename H, std::size_t S>
struct DBInfo
{
    String&         saltBase64;
    Digest<Hi>&     saltedPassword;
    Digest<H>&      storedKey;
    Digest<HMAC>&   serverKey;
    Digest<HMAC>&   clientKey;
    std::size_t     iteration;
};

template<typename Hi, typename HMAC, typename H, std::size_t S>
class ScramMechanism
{
    std::string_view normalize(std::string_view password)  {return password;}
    String          saltBase64;
    Digest<Hi>      saltedPassword;
    Digest<H>       storedKey;
    Digest<HMAC>    clientKey;
    Digest<HMAC>    serverKey;

    public:
    DBInfo<Hi, HMAC, H, S> makeAuthInfo(std::string_view password, std::string_view sb64, std::size_t iterationCount)
    {
        saltBase64 = sb64;
        using namespace std::literals;
        String salt(make_decode64(std::begin(saltBase64)), make_decode64(std::end(saltBase64)));

        Hi::hash(normalize(password), salt, iterationCount, saltedPassword);
        HMAC::hash(saltedPassword.view(), "Server Key"s, serverKey);
        HMAC::hash(saltedPassword.view(), "Client Key"s, clientKey);
        H::hash(clientKey.view(), storedKey);

        return DBInfo<Hi, HMAC, H, S>{saltBase64, saltedPassword, storedKey, serverKey, clientKey, iterationCount};
    }
};

struct ScramUtil
{
    static std::string getMessageBody(std::string const& section, std::string const& message)
    {
        auto findSection = message.find(section);
        if (findSection == std::string::npos || findSection + 2 >= message.size())
        {
            return "";
        }
        findSection += 2;
        auto sectionEnd = message.find(',', findSection);
        if (sectionEnd == std::string::npos)
        {
            sectionEnd = message.size();
        }
        return message.substr(findSection, (sectionEnd - findSection));
    }
    static String randomNonce()
    {
        std::uint8_t buf[16];
        if (RAND_bytes(buf, sizeof(buf)) != 1)
        {
            ThorsLogAndThrowError(std::runtime_error,
                                      "ThorsAnvil::Crypto::ScramUtil",
                                      "randomNonce",
                                      "RAND_bytes failed");
        }
        char* b = reinterpret_cast<char*>(buf);
        char* e = b + sizeof(buf);
        return String(make_encode64(b), make_encode64(e));
    }
    using CalcResp = std::tuple<String, String, String>;
    template<typename Hi, typename HMAC, typename H, std::size_t S>
    static CalcResp calcScram(std::string_view clientFirstMessage, std::string_view serverNonce, std::string_view serverFirstMessage, DBInfo<Hi, HMAC, H, S> const& info)
    {
        String     response("c=biws,r=");
        response += serverNonce;

        String     authMessage(clientFirstMessage.substr(3));
        authMessage += ",";
        authMessage += serverFirstMessage;
        authMessage += ",";
        authMessage += response;

        Digest<HMAC>    clientSignature;
        Digest<HMAC>    serverSignature;
        HMAC::hash(info.storedKey.view(), authMessage, clientSignature);
        HMAC::hash(info.serverKey.view(), authMessage, serverSignature);

        for (std::size_t loop = 0 ; loop < HMAC::digestSize; ++loop)
        {
            info.clientKey[loop] = info.clientKey[loop] ^ clientSignature[loop];
        }
        String proof(make_encode64(std::begin(info.clientKey)), make_encode64(std::end(info.clientKey)));
        String serverSignature64(make_encode64(std::begin(serverSignature)), make_encode64(std::end(serverSignature)));
        return {response, proof, serverSignature64};
    }
};

template<typename Hi, typename HMAC, typename H, std::size_t S>
class ScramClient
{
    std::string     user;
    String          password;
    String          nonce;
    String          serverSignature64;

    private:


    public:
        ScramClient(std::string const& user, std::string_view password, String nonce = ScramUtil::randomNonce())
            : user(user)
            , password(password)
            , nonce(std::move(nonce))
        {}
        std::string getFirstMessage()
        {
            using namespace std::literals;
            std::string result = "n,,n="s + user + ",r=";
            result += nonce;
            return result;
        }
        std::string getFinalMessage(std::string const& serverFirstMessage)
        {
            std::string serverNonce = ScramUtil::getMessageBody("r=", serverFirstMessage);
            std::string serverSalt  = ScramUtil::getMessageBody("s=", serverFirstMessage);
            std::size_t iteration   = std::min(100'000, std::stoi(ScramUtil::getMessageBody("i=", serverFirstMessage)));

            ScramMechanism<Hi, HMAC, H, S>  mechanism;
            DBInfo<Hi, HMAC, H, S> info = mechanism.makeAuthInfo(password, serverSalt, iteration);

            String proof;
            String response;
            std::tie(response, proof, serverSignature64) = ScramUtil::calcScram<Hi, HMAC, H, S>(getFirstMessage(), serverNonce, serverFirstMessage, info);

            std::string result;
            result += response;
            result += ",p=";
            result += proof;
            return result;
        }
        bool validateServer(std::string const& serverFinalMessage)
        {
            std::string     vFlag = ScramUtil::getMessageBody("v=", serverFinalMessage);
            return std::size(serverSignature64) == std::size(vFlag) && CRYPTO_memcmp(serverSignature64.data(), vFlag.data(), std::size(serverSignature64)) == 0;
        }
};

template<typename Hi, typename HMAC, typename H, std::size_t S>
class ScramServer
{
    std::function<DBInfo<Hi, HMAC, H, S>(std::string const&)>   dbAccess;
    std::function<String()>                                     nonceGenerator;
    String                                                      proof;
    String                                                      serverSignature64;
    public:
        template<typename F, typename N>
        ScramServer(F&& dbAccess, N&& nonceGenerator = [](){return ScramUtil::randomNonce();})
            : dbAccess(std::move(dbAccess))
            , nonceGenerator(std::move(nonceGenerator))
        {}
        std::string getFirstMessage(std::string const& clientFirstMessage)
        {
            std::string             user        = ScramUtil::getMessageBody("u=", clientFirstMessage);
            String                  nonce(std::string_view(ScramUtil::getMessageBody("r=", clientFirstMessage)));
            nonce += nonceGenerator();
            DBInfo<Hi, HMAC, H, S>  info        = dbAccess(user);

            std::string serverFirstMessage = "r=";
            serverFirstMessage += nonce;
            serverFirstMessage += ",s=";
            serverFirstMessage += info.saltBase64;
            serverFirstMessage += ",i=";
            serverFirstMessage += std::to_string(info.iteration);

            String response;
            std::tie(response, proof, serverSignature64) = ScramUtil::calcScram<Hi, HMAC, H, S>(clientFirstMessage, nonce, serverFirstMessage, info);

            return serverFirstMessage;
        }
        std::pair<bool, std::string> getFinalMessage(std::string const& clientFinalMessage)
        {
            std::string p = ScramUtil::getMessageBody("p=", clientFinalMessage);
            bool valid = p.size() == proof.size() && CRYPTO_memcmp(p.data(), proof.data(), p.size()) == 0;
            std::string sig = "v=";
            sig += serverSignature64;
            return {valid, sig};
        }
};

namespace V1
{

// These classes are depricated.
// They will be eventually replaced by the newer versions above.
// Please switch to the new API.
//
// Add V1 to older classes.

enum class DBInfoType
{
    Password,
    Salt
};

using NonceGenerator = std::function<std::string()>;
using DBInfoAccess   = std::function<std::string(DBInfoType, std::string const&)>;

// See below in ScramClient and SCramServer for examples of Hi/HMAC/H
// Hi   = Pbkdf2<HMac<Sha1>>
// HMAC = HMac<Sha1>
// H    = Sha1
template<typename Hi, typename HMAC, typename H, std::size_t S>
class ScramBase
{
    std::string getMessageBody(std::string const& section, std::string const& message) const
    {
        auto findSection = message.find(section);
        if (findSection == std::string::npos || findSection + 2 >= message.size())
        {
            return "";
        }
        findSection += 2;
        auto sectionEnd = message.find(',', findSection);
        if (sectionEnd == std::string::npos)
        {
            sectionEnd = message.size();
        }
        return message.substr(findSection, (sectionEnd - findSection));
    }

    std::string     clientFirstMessageBare;
    std::string     serviceFirstMessage;
    String          serverSignature64;
    String          clientProof64;
    NonceGenerator  nonceGenerator;

    public:
        ScramBase(std::string const& clientFirstMessageBare, NonceGenerator&& nonceGenerator)
            : clientFirstMessageBare(clientFirstMessageBare)
            , nonceGenerator(std::move(nonceGenerator))
        {}

    protected:
        std::size_t getServiceIteration()                                       const     {return std::stol(getMessageBody("i=", serviceFirstMessage));}
        std::string getUserFromMessage()                                        const     {return getMessageBody("n=", clientFirstMessageBare);}
        std::string getServiceSalt()                                            const     {return getMessageBody("s=", serviceFirstMessage);}
        std::string getClientNonce()                                            const     {return getMessageBody("r=", clientFirstMessageBare);}
        std::string getServiceNonce()                                           const     {return getMessageBody("r=", serviceFirstMessage);}
        std::string getVerification(std::string const& message)                 const     {return getMessageBody("v=", message);}
        std::string getProofFromClinet(std::string const& message)              const     {return getMessageBody("p=", message);}
        std::string normalize(std::string const& text)                          const     {return text;}

        std::string getClientFinalMessageWithoutProof()                         const     {return "c=biws,r=" + getServiceNonce();}
        std::string getAuthMessage()                                            const     {return clientFirstMessageBare + "," + serviceFirstMessage + "," + getClientFinalMessageWithoutProof();}

        std::string const& getClientFirstMessageBare()                          const     {return clientFirstMessageBare;}
        String const&      getClientProof()                                     const     {return clientProof64;}
        String const&      getServerSignature()                                 const     {return serverSignature64;}

        bool validateNonce(std::string const& message)                          const     {return getServiceNonce() == getMessageBody("r=", message);}
        void setServiceFirstMessage(std::string const& sfm)                               {serviceFirstMessage = sfm;}

        std::string generateNonce()                                                       {return nonceGenerator();}

        using HashDigest        = typename H::DigestStore;
        void calculateClientScramHash(std::string const& password)
        {
            Digest<Hi>      saltedPassword;
            Digest<HMAC>    clientKey;
            Digest<HMAC>    serverKey;
            Digest<HMAC>    clientSignature;
            Digest<HMAC>    serverSignature;
            Digest<H>       storedKey;

            using namespace std::literals;
            std::string    saltBase64      = getServiceSalt();
            String         salt              (make_decode64(std::begin(saltBase64)), make_decode64(std::end(saltBase64)));
            std::string    authMessage     = getAuthMessage();

            Hi::hash(normalize(password), salt, getServiceIteration(), saltedPassword);
            HMAC::hash(saltedPassword.view(), "Client Key"s, clientKey);
            HMAC::hash(saltedPassword.view(), "Server Key"s, serverKey);
            H::hash(clientKey, storedKey);
            HMAC::hash(storedKey.view(), authMessage, clientSignature);
            HMAC::hash(serverKey.view(), authMessage, serverSignature);

            for (std::size_t loop = 0 ; loop < HMAC::digestSize; ++loop)
            {
                clientKey[loop] = clientKey[loop] ^ clientSignature[loop];
            }
            clientProof64     = String(make_encode64(std::begin(clientKey)), make_encode64(std::end(clientKey)));
            serverSignature64 = String(make_encode64(std::begin(serverSignature)), make_encode64(std::end(serverSignature)));
        }
};

template<typename Hi, typename HMAC, typename H, std::size_t S>
class ScramClient: public ScramBase<Hi, HMAC, H, S>
{
    using Base = ScramBase<Hi, HMAC, H, S>;
    public:
        ScramClient(std::string const& userName, NonceGenerator&& nonceGenerator)
            : Base(std::string("n=") + userName + ",r=" + nonceGenerator(), std::move(nonceGenerator))
        {}
        std::string getFirstMessage()
        {
            using namespace std::literals;
            std::string fm = "n,,"s + Base::getClientFirstMessageBare();
            return fm;
        }
        std::string getProofMessage(std::string const& password, std::string const& sfm)
        {
            Base::setServiceFirstMessage(sfm);
            Base::calculateClientScramHash(password);
            std::string prof = Base::getClientFinalMessageWithoutProof();
            prof += ",p=";
            prof += Base::getClientProof();
            return prof;
        }
        bool verifyServer(std::string const& serverProof)
        {
            auto const& sig = Base::getServerSignature();
            auto        ver = Base::getVerification(serverProof);
            return sig.size() == ver.size() && CRYPTO_memcmp(sig.data(), ver.data(), sig.size()) == 0;
        }
};

template<typename Hi, typename HMAC, typename H, std::size_t S>
class ScramServer: public ScramBase<Hi, HMAC, H, S>
{
    using Base = ScramBase<Hi, HMAC, H, S>;

    long            iterationCount;
    DBInfoAccess    dbInfo;

    public:
        ScramServer(std::string const& clientFirstMessage,
                    std::size_t iterationCount,
                    NonceGenerator&& nonceGenerator,
                    DBInfoAccess&&   dbInfo)
            : Base(clientFirstMessage.substr(3), std::move(nonceGenerator))
            , iterationCount(iterationCount)
            , dbInfo(std::move(dbInfo))
        {}
        std::string getFirstMessage()
        {
            using namespace std::literals;
            std::string message = "r="s + Base::getClientNonce() + Base::generateNonce() + ",s="s + dbInfo(DBInfoType::Salt, Base::getUserFromMessage()) + ",i="s + std::to_string(iterationCount);
            Base::setServiceFirstMessage(message);
            return message;
        }
        std::string getProofMessage(std::string const& clientProof)
        {
            Base::calculateClientScramHash(Base::normalize(dbInfo(DBInfoType::Password, Base::getUserFromMessage())));
            auto const& cp = Base::getClientProof();
            auto        fp = Base::getProofFromClinet(clientProof);
            if (cp.size() == fp.size() && CRYPTO_memcmp(cp.data(), fp.data(), cp.size()) == 0 && Base::validateNonce(clientProof))
            {
                std::string prof = "v=";
                prof += Base::getServerSignature();
                return prof;
            }
            return "";
        }
};

using ScramClientSha1   = ScramClient<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;
using ScramServerSha1   = ScramServer<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;
using ScramClientSha256 = ScramClient<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;
using ScramServerSha256 = ScramServer<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;

} // End Namespace V1

using ScramClient1      = ScramClient<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;
using ScramServer1      = ScramServer<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;
using ScramMechanism1   = ScramMechanism<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;
using DBInfo1           = DBInfo<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;

using ScramClient256    = ScramClient<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;
using ScramServer256    = ScramServer<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;
using ScramMechanism256 = ScramMechanism<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;
using DBInfo256         = DBInfo<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;

}

#endif
