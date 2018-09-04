
#include "TCPServer.h"
#include "TCPClient.h"
#include <cstring>
#include <safe/Safe.h>

#ifdef __linux__
#include <sys/un.h>
#endif

using namespace communication;
using namespace communication::endpoint::tcp;

Server::Server() :
    TCPSocket(),
    IServer()
{
    m_listenQueueSize = 0;
    m_listenAddr.port = 0;
}
Server::~Server(){
}
int Server::writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj){    
    return 0;
}
int Server::readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj){    
    return 0;
}
int Server::setAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    
    switch(attrId){
        case Attribute_ListenAddressAndPort:{
            err = setDestination((struct AttributeListenAddressAndPort*)attr);
            break;
        }
        case Attribute_ListenQueueSize:{
            err = setListenQueueSize(*((int*)attr));
            break;
        }
        default:
            err = TCPSocket::setAttribute(attrId, attr, attrSize);
            break;
    }
    
    return err;
}
int Server::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    switch(attrId){
        case Attribute_ListenAddressAndPort:{
            err = getDestination((struct AttributeListenAddressAndPort*)attr);
            break;
        }
        case Attribute_ListenQueueSize:{
            err = getListenQueueSize((int*)attr);
            break;
        }
        default:
            err = TCPSocket::getAttribute(attrId, attr, attrSize);
            break;
    }
    return err;
}
int Server::startListen(){
    int err = -1;
    
    int fpos = m_listenAddr.address.find("unix:");
    if(fpos == 0){
        m_type = Type_UNIX;
    }
    
    if(m_sockID == SOCKET_INVALID){
        initSocket();
        if(m_sockID != SOCKET_INVALID){
            // bind
            struct sockaddr* addr = 0;
            socklen_t        addrlen = 0;
            
            if(m_type == Type_UNIX){
                #ifdef __linux__
                std::string szaddr = m_listenAddr.address;
                szaddr = szaddr.erase(0, 5);
                unlink(szaddr.c_str());
                
                addrlen = sizeof(struct sockaddr_un);
                addr = (struct sockaddr*)malloc(addrlen);
                memset(addr, 0, addrlen);
                ((struct sockaddr_un*)addr)->sun_family = AF_UNIX;
                cppframework::safe::s_memcpy(((struct sockaddr_un*)addr)->sun_path, szaddr.c_str(), szaddr.length()+1, 108);
                #endif
            }else{
                addrlen = sizeof(struct sockaddr_in);
                addr = (struct sockaddr*)malloc(addrlen);
                if (strcmp((const char*)m_listenAddr.address.c_str(), (const char*)"any") == 0){
                    ((struct sockaddr_in*)addr)->sin_addr.s_addr = htonl(INADDR_ANY);
                }
                else{
                    ((struct sockaddr_in*)addr)->sin_addr.s_addr = inet_addr((const char*)m_listenAddr.address.c_str());
                }
                ((struct sockaddr_in*)addr)->sin_family = AF_INET;
                ((struct sockaddr_in*)addr)->sin_port = htons(m_listenAddr.port);
            }
            if(addr){
                int flags = 1;
                setsockopt(m_sockID, SOL_SOCKET, SO_REUSEADDR, (char*)&flags, sizeof(int));
                err = bind(m_sockID, (const struct sockaddr*)addr, sizeof(struct sockaddr_in));
                if(err){
                    std::cout<<"bind failed ret "<<err<<" errno "<<errno<<std::endl;
                }
                free(addr);
            }
            err = listen(m_sockID, m_listenQueueSize);
            if(err){
                std::cout<<"listen failed ret "<<err<<" errno "<<errno<<std::endl;
            }

            if(err){
                destroySocket();
            }
        }
    }
    else err = 0;
    return err;
}
std::shared_ptr<ISocket> Server::acceptClient()
{
    if(m_sockID == SOCKET_INVALID) return nullptr;

    std::shared_ptr<ISocket> csock = nullptr;
    struct sockaddr *caddr = 0;
    socklen_t caddrlen = 0;
    
    if(m_type == Type_UNIX){
        #ifdef __linux__
        caddrlen = sizeof(struct sockaddr_un);
        #endif
    }else{
        caddrlen = sizeof(struct sockaddr_in);
    }
    if(caddrlen > 0)
        caddr = (struct sockaddr*)malloc(caddrlen);
    if(!caddr) return nullptr;

    int sockID = accept(m_sockID, caddr, &caddrlen);
    if(sockID >= 0){
        csock = makeClientInstance();
        csock->setSocketID(sockID);

        Client::AttributeDestination addr_port;
        if(m_type == Type_UNIX){
            #ifdef __linux__
            ((struct sockaddr_un*)caddr)->sun_path[107] = 0;
            addr_port.address = std::string(((struct sockaddr_un*)caddr)->sun_path);
            #endif
        }else{
            const char* szAddr = inet_ntoa(((struct sockaddr_in*)caddr)->sin_addr);
            addr_port.address = std::string(szAddr ? szAddr : "");
            addr_port.port    = ntohs(((struct sockaddr_in*)caddr)->sin_port);
        }
        csock->setAttribute(Client::Attribute_Destination, &addr_port);

        csock->setNonBlockingMode();
    }
    free(caddr);
    return csock;
}

int Server::setDestination(struct AttributeListenAddressAndPort* attr){
    m_listenAddr = *attr;
    return 0;
}
int Server::getDestination(struct AttributeListenAddressAndPort* attr){
    attr->address = m_listenAddr.address;
    attr->port  = m_listenAddr.port;
    return 0;
}
int Server::setListenQueueSize(int size){
    m_listenQueueSize = size;
    return 0;
}
int Server::getListenQueueSize(int* size){
    *size = m_listenQueueSize;
    return 0;
}
std::shared_ptr<communication::ISocket> Server::makeClientInstance(){
    return std::make_shared<Client>();
}
