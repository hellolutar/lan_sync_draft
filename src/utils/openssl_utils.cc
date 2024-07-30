#include "openssl_utils.h"

#include <filesystem>

#include "framework/itf/persist/persist_framework.h"

using namespace std;

OSSL_LIB_CTX *OpensslUtil::lib_ctx = OSSL_LIB_CTX_new();

// https://blog.csdn.net/QCZL_CC/article/details/119957918
string OpensslUtil::mdEncodeWithSHA3_512(string f)
{
    if (f.size() == 0)
        return "";

    std::filesystem::path fp(f);
    if (!filesystem::exists(fp) || filesystem::file_size(fp) == 0)
        return "";
    
    const char *option_properties = nullptr;
    EVP_MD *msg_digest = EVP_MD_fetch(lib_ctx, "SHA3-512", option_properties);

    assert(msg_digest);

    unsigned int digest_len = EVP_MD_get_size(msg_digest);
    assert(digest_len > 0);

    unsigned char *digest_value = (unsigned char *)OPENSSL_malloc(digest_len);

    assert(digest_value);

    EVP_MD_CTX *digest_ctx = EVP_MD_CTX_new();

    assert(digest_ctx);

    assert(EVP_DigestInit(digest_ctx, msg_digest) == 1);

    auto peg = PersistFramework::getEngine();
    uint64_t ret_data_len = 0;
    auto data_opt = peg->readFrom(f, ret_data_len);
    if (!data_opt.has_value())
    {
        // todo release
        EVP_MD_CTX_free(digest_ctx);
        OPENSSL_free(digest_value);
        EVP_MD_free(msg_digest);
        return "";
    }

    assert(EVP_DigestUpdate(digest_ctx, data_opt.value().get(), ret_data_len) == 1);

    assert(EVP_DigestFinal(digest_ctx, digest_value, &digest_len));

    stringstream iss;
    for (size_t j = 0; j < digest_len; j++)
    {
        char tmpBuf[3] = {0};
        sprintf(tmpBuf, "%02x", digest_value[j]);
        iss << tmpBuf;
    }
    string ret;
    iss >> ret;
    EVP_MD_CTX_free(digest_ctx);
    OPENSSL_free(digest_value);
    EVP_MD_free(msg_digest);

    return ret;
}