#include "crypto.h"
#include <cstring>
#include <iostream>

Crypto::Crypto():
m_keySize(32),
m_key(std::shared_ptr<uint8_t>(new uint8_t[m_keySize], [](uint8_t* p){ delete[] p; }))
{
    m_encryptCtx = std::shared_ptr<EVP_CIPHER_CTX>(EVP_CIPHER_CTX_new(), [](EVP_CIPHER_CTX* p){ EVP_CIPHER_CTX_free(p); });
    m_iv = std::shared_ptr<uint8_t>(new uint8_t[16], [](uint8_t* p) {delete[] p;});
}
Crypto::~Crypto(){
}


int Crypto::Decrypt(const void *encryptedData, int encryptedDataSize, const void *iv, void *_dataOut) {
    int ret = 0;
    int len;
    int ciphertext_len;
    uint8_t *dataOut = (uint8_t *)_dataOut;

    ret = EVP_DecryptInit_ex(m_encryptCtx.get(), EVP_aes_256_ecb(), NULL, (const uint8_t *)m_key.get(), (const uint8_t *)iv);
    if (ret != 1) {
        std::cout << "init error";
        return 0;
    }
    ret = EVP_DecryptUpdate(m_encryptCtx.get(), (unsigned char *)dataOut, &len, (const unsigned char *)encryptedData, encryptedDataSize);
    if (ret != 1) {
        std::cout << "update error";
        return 0;
    }
    ciphertext_len = len;
    ret = EVP_DecryptFinal_ex(m_encryptCtx.get(), (unsigned char *)(dataOut + len), &len);
    if (ret != 1) {
        std::cout << "final error" << ret;
        return 0;
    }
    ciphertext_len += len;

    return ciphertext_len;
}

int Crypto::SetKey(const void *key, int keySize) {
    memmove(m_key.get(), key, keySize);
    return 0;
}

int Crypto::DataDecrypt(const void *_dataInput, uint32_t dataLen, void *dataOutput)
{
    uint8_t *dataInput = (uint8_t*)_dataInput;
    uint32_t payloadLength;
    if(dataLen < 16) {
        std::cout << "invalid argument";
        return -1;
    }
    payloadLength = dataLen - 16;
    memcpy(m_iv.get(), dataInput, 16);
    return Decrypt(&dataInput[16], payloadLength, m_iv.get(), dataOutput);
}
