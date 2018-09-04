#include "TCPClient.h"
#include <errno.h>
#include <safe/Safe.h>

#ifdef __linux__
#include <sys/un.h>
#endif

using namespace communication;
using namespace communication::endpoint::tcp;

Client::Client() :
    TCPSocket(),
    IClient()
{
    m_destination.port = 0;
}
Client::~Client(){
}
int Client::writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int writtenLength = 0;
    
    if(m_sockID == SOCKET_INVALID) return writtenLength;
    
    writtenLength = send(m_sockID, (char*)data, dataSizeInByte, 0);    
    return writtenLength;
}
int Client::readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int readLength = 0;
    
    if(m_sockID == SOCKET_INVALID) return readLength;
    
    readLength = recv(m_sockID, (char*)data, dataSizeInByte, 0);
    
    return readLength;
}
int Client::setAttribute(int attrId, void* attr, int attrSize){
     int err = -1;
    
    switch(attrId){
        case Attribute_Destination:{
            err = setDestination((struct AttributeDestination*)attr);
            break;
        }
        default:
            err = TCPSocket::setAttribute(attrId, attr, attrSize);
            break;
    }
    
    return err;
}
int Client::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    
    switch(attrId){
        case Attribute_Destination:{
            err = getDestination((struct AttributeDestination*)attr);
            break;
        }
        default:
            err = TCPSocket::getAttribute(attrId, attr, attrSize);
            break;
    }
    
    return err;
}
int Client::connectToServer(int waitTimeoutInMilisecond){

    int err = -1;
    
    int fpos = m_destination.address.find("unix:");
    if(fpos == 0){
        m_type = Type_UNIX;
    }
    
    initSocket();
    if(m_sockID == SOCKET_INVALID)  {
        std::cout << "return \n";
        return -2;
    }
    size_t addrLen           = 0;
    struct sockaddr* addr    = 0;
    if(m_type == Type_UNIX){
        #ifdef __linux__
        std::string szaddr = m_destination.address;
        szaddr = szaddr.erase(0, 5);
        addrLen           = sizeof(struct sockaddr_un);
        addr    = (struct sockaddr*)malloc(addrLen);
        memset(addr, 0, addrLen);
        cppframework::safe::s_memcpy(((struct sockaddr_un*)addr)->sun_path, szaddr.c_str(), szaddr.length()+1, 108);
        ((struct sockaddr_un*)addr)->sun_family         = AF_UNIX;
        #endif
    }else{
        addrLen           = sizeof(struct sockaddr_in);
        addr    = (struct sockaddr*)malloc(addrLen);
        ((struct sockaddr_in*)addr)->sin_addr.s_addr    = inet_addr((const char*)m_destination.address.c_str());
        ((struct sockaddr_in*)addr)->sin_family         = AF_INET;
        ((struct sockaddr_in*)addr)->sin_port           = htons(m_destination.port);
    }
    if(addr){
        err = connect(m_sockID, addr, addrLen);        
        free(addr);
    }

    if(m_type == Type_UNIX) {
        std::cout << "type sock: " << m_type << err << std::endl;
        if(!err) return err;
    }

#ifdef __linux__
    if(errno != EINPROGRESS){
//         std::cout<<"errno = "<<errno<<" "<<m_destination.address<<" "<<m_destination.port<<std::endl;
        err = -3;
    }else{
#endif
        if(err != 0){
            int ret;
            struct timeval  tv;
            fd_set          fdw, fderror, fdr;

            tv.tv_sec  =  waitTimeoutInMilisecond/1000;
            tv.tv_usec = (waitTimeoutInMilisecond % 1000) * 1000;
            FD_ZERO(&fdw);
            FD_SET(m_sockID, &fdw);
            FD_ZERO(&fderror);
            FD_SET(m_sockID, &fderror);
            FD_ZERO(&fdr);
            FD_SET(m_sockID, &fdr);
            ret = select(m_sockID + 1, &fdr, &fdw, &fderror, &tv);
            //std::cout << "err " << err << std::endl;
            if (ret < 0 && errno != EINTR){
                err = -4;                
            }
            else if (ret > 0){
                if(FD_ISSET(m_sockID, &fderror)){
                    err = -5;
                }
                else if(FD_ISSET(m_sockID, &fdw) && FD_ISSET(m_sockID, &fdr)){
                    //err = -6;
                    err = 0;
                }
                else if(FD_ISSET(m_sockID, &fdw)){
                    err = 0; /* Haihv modified */
                    //err = -7;
                }
            }
            else{

            }
        }else{
            // Connect done immediately
            //std::cout << "Connect done immediately\n";
            err = 0;
        }
#ifdef __linux__
    }
#endif
    
    setNonBlockingMode();
    
    if(!err){
         //std::cout<<"tcp-client connect done, sockid "<<getSocketID()<<std::endl;
    }

    //std::cout << "Error: " << err << std::endl;
    
    return err;
}
int Client::disconnectToServer(){
    return destroySocket();
}
int Client::setDestination(struct AttributeDestination* attr){
    m_destination = *attr;
    //std::cout << "IP: " << m_destination.address << " PORT: " << m_destination.port << std::endl;
    return 0;
}
int Client::getDestination(struct AttributeDestination* attr){
    *attr = m_destination;
    return 0;
}
