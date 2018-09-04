#include "SSLClient.h"
#include "../../../cppframework.h"
#if COMMUNICATION_SOCKET_SSL_ENABLED > 0
#include <string>
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../../../safe/Safe.h"

#if defined(_MSC_VER)
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
#endif

namespace communication{
    namespace endpoint{
        namespace ssl{
            class ClientInternal{
            public:
                ClientInternal(bool init);
                virtual ~ClientInternal();
                
                int connectToServer(ISocket* sock);
                int disconnectToServer(ISocket* sock);
                int readData(void* data, int dataLength);
                int writeData(const void* data, int dataLength);
                
                int setSSLContext(SSL* _ssl);
                SSL* getSSLContext();
                
            private:
                SSL_CTX *ctx;               ///< SSL context
                SSL*     ssl;               ///< SSL connection context
            };
        }
    }
}

using namespace communication;
using namespace communication::endpoint::ssl;

ClientInternal::ClientInternal(bool init){
    ctx = 0;
    ssl = 0;
    if(init){
        ctx = SSL_CTX_new(TLSv1_client_method());
        ssl = SSL_new(ctx);
    }
}
ClientInternal::~ClientInternal(){
    if(ssl) SSL_free(ssl); ssl = 0;
    if(ctx) SSL_CTX_free(ctx); ctx = 0;
}
int ClientInternal::connectToServer(ISocket* sock){
    int err = -1;
    int sslret;
    if(!ssl) {
        std::cout << "Check OPENSSL LIBRARY IS PROPERTY INITALIZE\r\n";
        return err;
    }
    //perror("Start set FD ");
    SSL_set_fd(ssl, sock->getSocketID());
    //perror("Set FD  ");

    sslret = SSL_connect(ssl);

    switch(sslret) {
    case 0: /// Can not establish ssl connection
        std::cout << "TLS/SSL failed, shutdown controller" << std::endl;
        SSL_free(ssl);
        ssl = 0;

        break;
    case 1: /// No error
        err = 0;
        break;
    default: /// Other error
        perror("SSL_connect ");
        std::cout << "Reason " << SSL_get_error(ssl, sslret) << std::endl;
        SSL_free(ssl);
        ssl = 0;

        break;
    }

    return err;
}
int ClientInternal::disconnectToServer(ISocket* sock){
    if(ssl){
        SSL_shutdown(ssl);
        SSL_free(ssl); ssl = 0;
    }
    return 0;
}
int ClientInternal::setSSLContext(SSL* _ssl){
    ssl = _ssl;
    return 0;
}
SSL* ClientInternal::getSSLContext(){
    return ssl;
}
int ClientInternal::readData(void* data, int dataLength){
    if(!ssl) return 0;
    return SSL_read(ssl, data, dataLength);
}
int ClientInternal::writeData(const void* data, int dataLength){
    if(!ssl) return 0;    
    return SSL_write(ssl, data, dataLength);
}

int Client::setSSLContext(void* ctx){
    int err = -1;
    err = m_internal->setSSLContext((SSL*)ctx);
    return err;
}
Client::Client(bool initSSL) : communication::endpoint::tcp::Client() {
    //m_internal = std::make_shared<ClientInternal>(initSSL);
    m_internal = nullptr;
}
Client::~Client(){
}
int Client::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    switch(attrId){
        case Attribute_SSL_INFO:{
            err = getInformation((struct AttributeSSLInfo*)attr);
            break;
        }
        default:
            err = tcp::Client::getAttribute(attrId, attr, attrSize);
            break;
    }
    return err;
}
int Client::writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    return m_internal->writeData(data, dataSizeInByte);
}
int Client::readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    return m_internal->readData(data, dataSizeInByte);
}
int Client::connectToServer(int waitTimeoutInMilisecond){
    int err = communication::endpoint::tcp::Client::connectToServer(waitTimeoutInMilisecond);
    //std::cout << "TCP Err " << err << std::endl;
    if(!err){
        m_internal = std::make_shared<ClientInternal>(true);
        setBlockingMode();
        err = m_internal->connectToServer(this);
        setNonBlockingMode();
    }
    return err;
}
int Client::disconnectToServer(){
    int err = 0;
    /* 171110 modified for multi time connect/disconnect to server */
    if(m_internal) {
        err = m_internal->disconnectToServer(this);
        m_internal.reset();
    }
    err = communication::endpoint::tcp::Client::disconnectToServer();

    return err;
}
int Client::getInformation(struct AttributeSSLInfo* info){
    X509* x509 = SSL_get_peer_certificate(m_internal->getSSLContext());
    EVP_PKEY* pkey = X509_get_pubkey(x509);
    BIO  *outbio = NULL;
    //char *line;
    const char* cline;
    const SSL_CIPHER* cipher;
    int ret;

    memset(info, 0, sizeof(struct AttributeSSLInfo));

    outbio = BIO_new(BIO_s_mem());
    /*switch (pkey->type) {
        case EVP_PKEY_RSA:
            info->keyType = KEY_TYPE_RSA;
            info->keyBitLength = EVP_PKEY_bits(pkey);
            break;
        case EVP_PKEY_DSA:
            info->keyType = KEY_TYPE_DSA;
            info->keyBitLength = EVP_PKEY_bits(pkey);
            break;
        default:
            info->keyType = KEY_TYPE_UNKNOWN;
            info->keyBitLength = EVP_PKEY_bits(pkey);
            break;
    }*/
    PEM_write_bio_PUBKEY(outbio, pkey);
    BIO_read(outbio, info->keyPublic, KEY_PUBLIC_LENGTH-1);
    X509_NAME_oneline(X509_get_subject_name(x509), (char*)info->subjectName, SUBJECT_NAME_LENGTH-1);
    BIO_free_all(outbio);
    EVP_PKEY_free(pkey);
    X509_free(x509);

    cipher = SSL_get_current_cipher(m_internal->getSSLContext());
    cline = SSL_CIPHER_get_name(cipher);
    if(cline){
        std::string sline = std::string(cline);
        cppframework::safe::s_memcpy(info->cipherName, cline, sline.length(), CIPHER_NAME_LENGTH);
    }
    SSL_CIPHER_get_bits(cipher, &ret);
    info->cipherBitLength = ret;
    return 0;
}
#endif
// end of file
