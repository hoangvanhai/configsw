#include "TCPSocket.h"
#include "../SocketUtils.h"

using namespace communication;
using namespace communication::endpoint::tcp;

TCPSocket::TCPSocket() :
    ISocket(),
    m_type(Type_INET)
{
    m_sockID = SOCKET_INVALID;
    m_bufferSize.recvBufferSize = 1024*1024;
    m_bufferSize.sendBufferSize = 1024*1024;
}
TCPSocket::~TCPSocket(){
    destroySocket();
}
int TCPSocket::initSocket(){
    int err = 0;
    if(m_sockID == SOCKET_INVALID){
        if(m_type == Type_UNIX){
#ifdef __linux__
            m_sockID = socket(AF_UNIX, SOCK_STREAM, IPPROTO_IP);
#endif
        }else{
            #ifdef WIN32
            // Initialize Winsock dll
            int err;
            WSADATA wsaData;
            err = WSAStartup(MAKEWORD(2,2), &wsaData);
            if (err) {
                std::cout << "WSAStartup failed: " << err ;
                return err;
            }
            #endif
            m_sockID = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        }
        if(m_sockID != SOCKET_INVALID){
            setNonBlockingMode();
            utils::Network::setBufferSize(this, m_bufferSize.recvBufferSize, m_bufferSize.sendBufferSize);
            err = 0;
        }
    }else{
    }
    return err;
}
int TCPSocket::destroySocket(){
    if(m_sockID != SOCKET_INVALID){
#if defined(__linux__)
		close(m_sockID);
#elif defined(_MSC_VER)
		closesocket(m_sockID);
#endif
        m_sockID = SOCKET_INVALID;
    }
    return 0;
}
int TCPSocket::setAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    
    (void)attrSize;

    switch(attrId){
        case Attribute_BufferSize:{
            struct AttributeBufferSize* bufSize = (struct AttributeBufferSize*)attr;
            err = setBufferSize(bufSize->recvBufferSize, bufSize->sendBufferSize);
            break;
        }
    }
    
    return err;
}
int TCPSocket::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    (void)attrSize;
    switch(attrId){
        case Attribute_BufferSize:{
            struct AttributeBufferSize* bufSize = (struct AttributeBufferSize*)attr;
            err = getBufferSize(&(bufSize->recvBufferSize), &(bufSize->sendBufferSize));
            break;
        }
    }
    return err;
}
int TCPSocket::setBufferSize(int recvBufferSizeInByte, int sendBufferSizeInByte){
    m_bufferSize.recvBufferSize = recvBufferSizeInByte;
    m_bufferSize.sendBufferSize = sendBufferSizeInByte;
    return 0;
}
int TCPSocket::getBufferSize(int *recvBufferSizeInByte, int *sendBufferSizeInByte){
    if(m_sockID != SOCKET_INVALID){
        return utils::Network::getBufferSize(this, recvBufferSizeInByte, sendBufferSizeInByte);
    }
    *recvBufferSizeInByte = 0;
    *sendBufferSizeInByte = 0;
    return 0;
}
