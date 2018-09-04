#include "ISocket.h"

using namespace communication;

#include <fcntl.h>
#if defined(__linux__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#elif defined(_MSC_VER)
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#endif

Error::Error(int code) : m_errorCode(code) {}
Error::~Error(){}

int  Error::code(){return m_errorCode;}
void Error::setCode(int code){ m_errorCode = code;}
std::string Error::toString(){
    switch(m_errorCode){
        case ErrorCode_None:
            return "none";
            break;
        default:
            return "unknown";
            break;
    }
    return "unknown";
}
ISocket::ISocket():
m_error(std::make_shared<Error>(ErrorCode_None)),
m_sockID(SOCKET_INVALID)
{
    m_lastEvent = 0;
}
ISocket::~ISocket(){
    if(m_sockID != SOCKET_INVALID){
#if defined(__linux__)
        close(m_sockID);
#elif defined(_MSC_VER)
		closesocket(m_sockID);
#endif
        m_sockID = SOCKET_INVALID;
    }
}
std::shared_ptr<Error> ISocket::getLastError(){return m_error;}
int ISocket::waitEvent(int eventMustToWait, int timeoutInMilisecond){
    int             event = Event_Timeout;
    
    struct timeval  tv;
    fd_set          fd_read, fd_write, fd_error;
    
    if(m_sockID == SOCKET_INVALID) return event;

    tv.tv_sec   =  timeoutInMilisecond / 1000;
    tv.tv_usec  = (timeoutInMilisecond % 1000) * 1000;
    FD_ZERO(&fd_read);
    FD_ZERO(&fd_write);
    FD_ZERO(&fd_error);

    FD_SET(m_sockID, &fd_read);
    FD_SET(m_sockID, &fd_write);
    FD_SET(m_sockID, &fd_error);
    
    fd_set *r = 0, *w = 0;
    if(eventMustToWait & Event_Readable) r = &fd_read;
    if(eventMustToWait & Event_Writable) w = &fd_write;
    
    int ret = 0;
    do{
        ret = select(m_sockID + 1, r, w, &fd_error, &tv);
        if((ret < 0) && (errno == EINTR))
            continue;
        break;
    }while(1);
    if(ret < 0){
        // error
        if(errno == EINTR){
            event = Event_Timeout;
        }else
            event = Event_Error;
    }
    else if(ret == 0){
        // timeout
        event = Event_Timeout;
    }
    else{
        event = 0;
        if (FD_ISSET(m_sockID, &fd_error)){
            event |= Event_Error;
        }
        if (r && (FD_ISSET(m_sockID, &fd_read))){
            event |= Event_Readable;
        }        
        if (w && (FD_ISSET(m_sockID, &fd_write))){
            event |= Event_Writable;
        }
    }
    return event;
}
int ISocket::setBlockingMode(){
    int err = -1;
    if(m_sockID == SOCKET_INVALID) return err;
#if defined(__linux__)
    int val = fcntl(m_sockID, F_GETFL, NULL);
    if(val >= 0){
        val &= ~O_NONBLOCK;
        val = fcntl(m_sockID, F_SETFL, val);
        if(val >= 0)
            err = 0;
    }
#elif defined(_MSC_VER)
	unsigned long mode = 0;
	ioctlsocket(m_sockID, FIONBIO, &mode);
	err = 0;
#endif
    return err;
}
int ISocket::setNonBlockingMode(){
    int err = -1;
    if(m_sockID == SOCKET_INVALID) return err;
#if defined(__linux__)
    int val = fcntl(m_sockID, F_GETFL, NULL);
    if(val >= 0){
        val |= O_NONBLOCK;
        val = fcntl(m_sockID, F_SETFL, val);
        if(val >= 0)
            err = 0;
    }
#elif defined(_MSC_VER)
	unsigned long mode = 1;
	ioctlsocket(m_sockID, FIONBIO, &mode);
	err = 0;
#endif
    
    return err;
}
void ISocket::setSocketID(int id){ m_sockID = id;}
int  ISocket::getSocketID(){ return m_sockID; }
void ISocket::setErrorCode(enum ErrorCode code){ m_error->setCode(code);}
int  ISocket::lastEvent(){ return m_lastEvent; }
int  ISocket::waitMultiEvent(int eventMustToWait, int timeoutInMilisecond, std::list<std::shared_ptr<ISocket>> socks){
    int             event = Event_Timeout;
    
    struct timeval  tv;
    fd_set          fd_read, fd_write, fd_error;

    tv.tv_sec   =  timeoutInMilisecond / 1000;
    tv.tv_usec  = (timeoutInMilisecond % 1000) * 1000;
    FD_ZERO(&fd_read);
    FD_ZERO(&fd_write);
    FD_ZERO(&fd_error);
    
    int maxID = 0;
    for(auto sock : socks){
        int id = sock->getSocketID();
        FD_SET(id, &fd_read);
        FD_SET(id, &fd_write);
        FD_SET(id, &fd_error);
        if(id > maxID) maxID = id;
        sock->m_lastEvent = 0;
    }
    
    fd_set *r = 0, *w = 0;
    if(eventMustToWait & Event_Readable) r = &fd_read;
    if(eventMustToWait & Event_Writable) w = &fd_write;
    
    int ret = select(maxID + 1, r, w, &fd_error, &tv);
    if(ret < 0){
        // error
        event = Event_Error;
    }
    else if(ret == 0){
        // timeout
        event = Event_Timeout;
    }
    else{
        event = 0;
        for(auto sock : socks){
            int id = sock->getSocketID();
            if (FD_ISSET(id, &fd_error)){
                event |= Event_Error;
                sock->m_lastEvent |= Event_Error;
            }
            if (r && (FD_ISSET(id, &fd_read))){
                event |= Event_Readable;
                sock->m_lastEvent |= Event_Readable;
            }        
            if (w && (FD_ISSET(id, &fd_write))){
                event |= Event_Writable;
                sock->m_lastEvent |= Event_Writable;
            }
        }
    }
    return event;
}
// end of file

