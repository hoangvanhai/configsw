#include "SocketUtils.h"
#if defined(__linux__)
#include <sys/types.h>
#include <sys/socket.h>
#elif defined(_MSC_VER)
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#endif

using namespace communication;
using namespace communication::utils;

Network::Network(){}
Network::~Network(){}
/**
 * @note buffer size will be double in setsockopt
 */ 
int Network::setBufferSize(ISocket* sock, int recvBufferSizeInByte, int sendBufferSizeInByte){
    int err = 0;
#ifdef __linux__
    int         ival        = recvBufferSizeInByte / 2;
#else
	int         ival = recvBufferSizeInByte;
#endif
    socklen_t   bufsizelen  = sizeof(ival);
    if(ival > 0)
        err = setsockopt(sock->getSocketID(), SOL_SOCKET, SO_RCVBUF, (char*)&ival, bufsizelen);
#ifdef __linux__
	ival = sendBufferSizeInByte / 2;
#else
	ival = sendBufferSizeInByte;
#endif
    if(ival > 0)
        err = err || setsockopt(sock->getSocketID(), SOL_SOCKET, SO_SNDBUF, (char*)&ival, bufsizelen);
    return err;
}
int Network::getBufferSize(ISocket* sock, int *recvBufferSizeInByte, int *sendBufferSizeInByte){
    int err = 0;
    int         ival;
    socklen_t   bufsizelen;
    
    ival = 0;
    bufsizelen  = sizeof(ival);
    if(getsockopt(sock->getSocketID(), SOL_SOCKET, SO_RCVBUF, (char*)&ival, &bufsizelen)){
        err = -1;
    }else{
        *recvBufferSizeInByte = ival;
    }
    
    ival = 0;
    bufsizelen  = sizeof(ival);
    if(getsockopt(sock->getSocketID(), SOL_SOCKET, SO_SNDBUF, (char*)&ival, &bufsizelen)){
        err = -1;
    }else{
        *sendBufferSizeInByte = ival;
    }
    
    return err;
}
/* CRC */
CRC::CRC(){
    // crc16
    m_crc16.polynomial = 0x8005;
    for (int i = 0; i < 0x10000; i++){
        uint16_t c = (uint16_t)i;
        for (int j = 0; j < 8; j++)
        {
            uint16_t t = (uint16_t)(c & 0x8000);
            c = (uint16_t)(c << 1);

            if (t == 0x8000)
            {
                c = (uint16_t)(c ^ m_crc16.polynomial);
            }
        }
        m_crc16.tbl[i] = c;
    }
}
CRC::~CRC(){}
uint16_t CRC::crc16(uint16_t init, const void* data, int dataLength){
    std::shared_ptr<CRC> inst = getInstance();
    if(inst == nullptr) return 0;
    
    uint8_t* u8p = (uint8_t*)data;
    int16_t* tbl = inst->m_crc16.tbl;
    while(dataLength--){
        init = tbl[init ^ ((uint16_t)(*u8p) << 8)];
        u8p++;
    }
    return init;
}
std::shared_ptr<CRC> CRC::getInstance(){
    std::call_once(m_initFlag, [&](){
        m_inst = std::make_shared<CRC>();
    });
    return m_inst;
}
void CRC::destroyInstance(){
    //m_inst.reset();
}
std::shared_ptr<CRC> CRC::m_inst;
std::once_flag       CRC::m_initFlag;
