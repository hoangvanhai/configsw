#ifndef CRYPTO_H
#define CRYPTO_H

#include <memory>
#include <cstdint>

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

class Crypto {
    public:
        Crypto();
        virtual ~Crypto();

        const std::shared_ptr<uint8_t> Key(){ return m_key; }
        int KeySize(){ return m_keySize; }

        int Decrypt(const void *encryptedData, int encryptedDataSize, const void *iv, void *_dataOut);
        int SetKey(const void *key, int keySize);
        int DataDecrypt(const void *_dataInput, uint32_t dataLen, void *dataOutput);
private:
        int m_keySize;
        std::shared_ptr<uint8_t> m_key;

        std::shared_ptr<EVP_CIPHER_CTX> m_encryptCtx;
        std::shared_ptr<uint8_t>        m_iv;
};

#endif // CRYPTO_H
