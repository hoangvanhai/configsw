#include "UDPSocket.h"
#include "../SocketUtils.h"

using namespace communication;
using namespace communication::endpoint::udp;

Socket::Socket() : ISocket() {
#ifdef WIN32
    // Initialize Winsock dll
    int err;
    WSADATA wsaData;
    err = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (err) {
        std::cout << "WSAStartup failed: " << err ;
    }
#endif

    //m_sockID = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    m_sockID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setNonBlockingMode();
}
Socket::~Socket(){
    
}
int Socket::writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int writeLength = 0;
    
    if(m_sockID == SOCKET_INVALID) return writeLength;
    
    std::shared_ptr<communication::endpoint::udp::Destination> dest = std::dynamic_pointer_cast<communication::endpoint::udp::Destination>(obj);
    if(dest){
        socklen_t len = sizeof(struct sockaddr_in);
        writeLength = sendto(m_sockID, (char*)data, dataSizeInByte, 0, (struct sockaddr*)dest->getAddrPointer(), len);
    }else{
        std::cout<<"udp-server: dynamic cast failed"<<std::endl;
    }    
    return writeLength;
}
int Socket::readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int readLength = 0;
    
    if(m_sockID == SOCKET_INVALID) return readLength;
    
    std::shared_ptr<communication::endpoint::udp::Destination> dest = std::dynamic_pointer_cast<communication::endpoint::udp::Destination>(obj);
    if(dest){
        socklen_t len = sizeof(struct sockaddr_in);
        readLength = recvfrom(m_sockID, (char*)data, dataSizeInByte, 0, (struct sockaddr*)dest->getAddrPointer(), &len);
    }else{
        std::cout<<"udp-server: dynamic cast failed"<<std::endl;
    }
    
    return readLength;
}
int Socket::setAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    switch(attrId){
        case Attribute_ListenAddressAndPort:{            
            err = setListenAddressAndPort((struct AttributeListenAddressAndPort*)attr);
            break;
        }
        case Attribute_BufferSize:{
            struct AttributeBufferSize* bufSize = (struct AttributeBufferSize*)attr;
            err = setBufferSize(bufSize->recvBufferSize, bufSize->sendBufferSize);
            break;
        }
        case Attribute_Broadcast:{
            bool* val = (bool*)attr;
            err = setBroadcast(*val);
            break;
        }
        default:
            std::cout << "NOT HANDLED CASE\r\n";
            break;
    }
    
    return err;
}
int Socket::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
        
    switch(attrId){
        case Attribute_BufferSize:{
            struct AttributeBufferSize* bufSize = (struct AttributeBufferSize*)attr;
            err = getBufferSize(&(bufSize->recvBufferSize), &(bufSize->sendBufferSize));
            break;
        }
        default:
            break;
    }
    
    return err;
}
int Socket::setBroadcast(bool val){
    int err = -1;
    if(m_sockID == SOCKET_INVALID) return err;
    
    int opt = val ? 1 : 0;
    err = setsockopt(m_sockID, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(opt));
    
    return err;
}
int Socket::setListenAddressAndPort(struct AttributeListenAddressAndPort *dest){
    int err = -1;
    
    if(m_sockID == SOCKET_INVALID) return err;
    
    if (dest->address == "any"){
        m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else{
        m_addr.sin_addr.s_addr = inet_addr(dest->address.c_str());
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(dest->port);    
    err = bind(m_sockID, (struct sockaddr*)&m_addr, sizeof(m_addr));    
    return err;
}
int Socket::setBufferSize(int recvBufferSizeInByte, int sendBufferSizeInByte){
    return utils::Network::setBufferSize(this, recvBufferSizeInByte, sendBufferSizeInByte);
}
int Socket::getBufferSize(int *recvBufferSizeInByte, int *sendBufferSizeInByte){
    return utils::Network::getBufferSize(this, recvBufferSizeInByte, sendBufferSizeInByte);
}

Destination::Destination(std::string address, int port) : Object(){
    setAddressAndPort(address, port);
}
Destination::~Destination(){
    
}
std::string Destination::getAddress(){
    std::string address;
    
    const char* sz = inet_ntoa(m_addr.sin_addr);
    if(sz) address = std::string(sz);
    
    return address;
}
int Destination::getPort(){
    int port = 0;
    
    port = ntohs(m_addr.sin_port);
    
    return port;
}
int Destination::setAddressAndPort(std::string address, int port){
    int err = -1;
    
    if (address == "any"){
        m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }else if (address == "broadcast"){
        m_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    }else{
        m_addr.sin_addr.s_addr = inet_addr(address.c_str());
    }
    m_addr.sin_family   = AF_INET;
    m_addr.sin_port     = htons(port);
    
    
    err = 0;
    
    return err;
}
