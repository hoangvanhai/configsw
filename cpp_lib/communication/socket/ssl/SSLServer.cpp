
#include "SSLServer.h"
#include "SSLClient.h"
#include "../../../cppframework.h"
#if COMMUNICATION_SOCKET_SSL_ENABLED > 0
#include <string>
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>

#if defined(_MSC_VER)
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
#endif

namespace communication{
    namespace endpoint{
        namespace ssl{
            class ServerInternal{
            public:
                ServerInternal();
                virtual ~ServerInternal();
                
                int setCertificationAndKeyFile(std::string &certFile, std::string &keyFile);
                SSL* acceptClient(std::shared_ptr<Client> sock);
                            
            private:
                SSL_CTX *ctx; ///< SSL context
            };
        }
    }
}
using namespace communication;
using namespace communication::endpoint::ssl;

ServerInternal::ServerInternal(){
    ctx = SSL_CTX_new(TLSv1_server_method());
}
ServerInternal::~ServerInternal(){
    if(ctx) SSL_CTX_free(ctx); ctx = 0;
}
int ServerInternal::setCertificationAndKeyFile(std::string &certFile, std::string &keyFile){
    int err = -1;
    if(!ctx) return err;
    
    err = 0;
    if (SSL_CTX_use_certificate_file(ctx, certFile.c_str(), SSL_FILETYPE_PEM) <= 0){
        err = -2;
    }
    if(!err){
        if (SSL_CTX_use_PrivateKey_file(ctx, keyFile.c_str(), SSL_FILETYPE_PEM) <= 0){
            err = -3;
        }
    }
    if(!err){
        if (!SSL_CTX_check_private_key(ctx)){
            err = -4;
        }
    }
    return err;
}
SSL* ServerInternal::acceptClient(std::shared_ptr<Client> sock){
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock->getSocketID());
    if(SSL_accept(ssl) == -1){
        std::cout<<"ssl-server accept failed, sockid "<<sock->getSocketID()<<std::endl;
//         ERR_print_errors_fp(stdout);
        SSL_free(ssl);
        ssl = 0;
    }
    
    return ssl;
}

Server::Server() : communication::endpoint::tcp::Server()
{
    m_internal = std::make_shared<ServerInternal>();
}
Server::~Server(){
}
int Server::setAttribute(int attrId, void* attr, int attrSize){
    int err = 0;
    switch(attrId){
        case Attribute_SSL_CERT_KEY:{
            struct AttributeCertAndKey* cert_key = (struct AttributeCertAndKey*)attr;
            err = m_internal->setCertificationAndKeyFile(cert_key->certFile, cert_key->keyFile);
            break;
        }
        default:{
            err = communication::endpoint::tcp::Server::setAttribute(attrId, attr, attrSize);
            break;
        }
    }
    return err;
}
int Server::getAttribute(int attrId, void* attr, int attrSize){
    return communication::endpoint::tcp::Server::getAttribute(attrId, attr, attrSize);
}
std::shared_ptr<ISocket> Server::acceptClient(){
    std::shared_ptr<ISocket> isock = communication::endpoint::tcp::Server::acceptClient();
    if(isock != nullptr){
        std::shared_ptr<Client> sock = std::dynamic_pointer_cast<Client>(isock);
        if(sock){
            isock->setBlockingMode();
            SSL* ssl = m_internal->acceptClient(sock);
            if(ssl){
                sock->setSSLContext(ssl);
                isock->setNonBlockingMode();
            }else{
                isock = nullptr;
            }
        }else{
//             std::cout<<"cast ISock to ssl::Client failed : "<<isock.get()<<std::endl;
            isock = nullptr;
        }
    }
    return isock;
}
std::shared_ptr<communication::ISocket> Server::makeClientInstance(){
    std::shared_ptr<communication::ISocket> sock = std::make_shared<communication::endpoint::ssl::Client>(false);
//     std::cout<<"Make ssl::Client : "<<sock.get()<<std::endl;
    return sock;
}
#endif
// end of file
