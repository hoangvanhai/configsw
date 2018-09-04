#include "EthernetSocket.h"
#include "../../../cppframework.h"
#include "../SocketUtils.h"

using namespace communication;
using namespace communication::endpoint::ethernet;

#if COMMUNICATION_SOCKET_ETHERNET_ENABLED == 1

std::string Socket::getInterface(){
    return m_ifname;
}
int Socket::setAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    switch (attrId){
    case Attribute_Interface:{
        struct AttributeInterface *ifname = (struct AttributeInterface*)attr;
        err = setInterface(ifname->ifname);
        break;
    }
    case Attribute_BufferSize:{
        struct AttributeBufferSize* size = (struct AttributeBufferSize*)attr;
        err = setBufferSize(size->recvBufferSize, size->sendBufferSize, size->blockSize);
        break;
    }
    default:
        break;
    }
    return err;
}
int Socket::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    switch (attrId){
    case Attribute_Interface:{
        struct AttributeInterface *ifname = (struct AttributeInterface*)attr;
        ifname->ifname = getInterface();
        err = 0;
        break;
    }
    case Attribute_BufferSize:{
        struct AttributeBufferSize* size = (struct AttributeBufferSize*)attr;
        err = getBufferSize(&size->recvBufferSize, &size->sendBufferSize, &size->blockSize);
        break;
    }
    default:
        break;
    }
    return err;
}
ReceivedDataObject::ReceivedDataObject() : Object(){}
ReceivedDataObject::~ReceivedDataObject(){}
#endif