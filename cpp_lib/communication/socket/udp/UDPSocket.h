/**
 * @addtogroup communication_endpoint
 * @{
 */
#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "../ISocket.h"
#include <string>

#if defined(__linux__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#elif defined(_MSC_VER)
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace communication{
    namespace endpoint{
        namespace udp{
            /**
             * @brief Destination information
             */ 
            class Destination : public Object{
            public:
                Destination(std::string address="", int port=0);
                virtual ~Destination();
                /**
                 * @brief Get IP address
                 */ 
                std::string getAddress();
                /**
                 * @brief Get port
                 */ 
                int         getPort();
                /**
                 * @brief Set IP address and port
                 */ 
                int         setAddressAndPort(std::string address, int port);
                
                struct sockaddr_in* getAddrPointer(){ return &m_addr; }
            private:
                struct sockaddr_in m_addr;
            };
            /**
             * @brief Attributes
             */ 
            enum Attribute_ID{
                Attribute_ListenAddressAndPort = 0, ///< Listen address and port
                Attribute_BufferSize,               ///< Buffers size
                Attribute_Broadcast,                ///< Broadcast mode, attr = &bool
            };
            /**
             * @brief Attributes for Attribute_ListenAddressAndPort
             */ 
            struct AttributeListenAddressAndPort{
                std::string address;    ///< Listen IP address
                int port;               ///< Listen port
            };
            /**
             * @brief Attributes for Attribute_BufferSize
             */ 
            struct AttributeBufferSize{
                int recvBufferSize;     ///< Received buffer size
                int sendBufferSize;     ///< Send buffer size
            };
            class Socket : public ISocket{
            public:
                Socket();
                virtual ~Socket();
                
                /* Override ISocket */
                virtual int writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj);
                virtual int readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj);
                virtual int setAttribute(int attrId, void* attr, int attrSize);
                virtual int getAttribute(int attrId, void* attr, int attrSize);
                
            private:
                struct sockaddr_in  m_addr;
                int         setListenAddressAndPort(struct AttributeListenAddressAndPort *dest);
                int         setBroadcast(bool val);
                virtual int setBufferSize(int recvBufferSizeInByte, int sendBufferSizeInByte);
                virtual int getBufferSize(int *recvBufferSizeInByte, int *sendBufferSizeInByte);
                
            };
        }
    }
}
#endif // UDPSOCKET_H

/**
 * @}
 */ 
