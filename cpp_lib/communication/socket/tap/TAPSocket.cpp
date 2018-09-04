#include "TAPSocket.h"
#if defined(__linux__)
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>

#include <cstring>
#include "../../../dll/dll_function.h"
#include "../../../safe/Safe.h"

using namespace communication::endpoint::tap;

TAPSocket::TAPSocket() : ISocket() {
}
TAPSocket::~TAPSocket(){
    
}
int TAPSocket::writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int writeLength = 0;
    
    if(m_sockID == SOCKET_INVALID) return writeLength;
    
    writeLength = write(m_sockID, data, dataSizeInByte);
    
    return writeLength;
}
int TAPSocket::readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int readLength = 0;
    
    if(m_sockID == SOCKET_INVALID) return readLength;
    read_dll _read = dll_function<read_dll>("read");
    readLength = _read(m_sockID, data, dataSizeInByte);
    
    return readLength;
}
int TAPSocket::setAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    
    switch(attrId){
        case Attribute_PortName:{
            err = setPortName((struct AttributePortName*)attr);
            break;
        }
        default:
            break;
    }
    
    return err;
}
int TAPSocket::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
        
    switch(attrId){
        case Attribute_PortName:{
            err = getPortName((struct AttributePortName*)attr);
            break;
        }
        default:
            break;
    }
    
    return err;
}

int TAPSocket::setPortName(struct AttributePortName *port){
    int err = -1;
    open_dll _open = dll_function<open_dll>("open");
    m_sockID = _open("/dev/net/tun", O_RDWR, 0);
    if(m_sockID != SOCKET_INVALID){
        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
            ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
            cppframework::safe::s_memcpy(ifr.ifr_name, port->portname.c_str(), port->portname.length(), IFNAMSIZ);
            err = ioctl(m_sockID, TUNSETIFF, (void *)&ifr);
            if(err < 0){
                close(m_sockID);
                m_sockID = SOCKET_INVALID;
            }else{
                err = 0;
            }
    }
   
    return err;
}
int TAPSocket::getPortName(struct AttributePortName *port){
    port->portname = m_portName;
    return 0;
}
#endif
// end of file
