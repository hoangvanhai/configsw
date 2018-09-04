/**
 * @addtogroup communication_endpoint
 * @{
 */
#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include "../ISocket.h"

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
        namespace tcp{
            /**
             * @brief Attributes
             */ 
            enum Attribute_TCP_SOCKET_ID{
                Attribute_BufferSize,           ///< Buffer size
                Attribute_TCP_SOCKET_ID_END
            };
            /**
             * @brief Attributes for Attribute_BufferSize
             */ 
            struct AttributeBufferSize{
                int recvBufferSize;     ///< Received buffer size
                int sendBufferSize;     ///< Send buffer size
            };
            /**
             * @brief Implement TCP socket
             *  Attributes
                @code
                +-----------------------------------+----------------------------------------+------------+---------------------------------+---------+
                |             ID                     |           Attribute                   |   Length   |            Desc                 | Action  |
                +------------------------------------+---------------------------------------+------------+---------------------------------+---------+
                | Attribute_BufferSize               | struct AttributeBufferSize&           |   ignore   | setup recv & send buffer size   | Get/Set |
                +------------------------------------+---------------------------------------+------------+---------------------------------+---------+
                @endcode
            */ 
            class TCPSocket : public ISocket{
            public:
                TCPSocket();
                virtual ~TCPSocket();
                
                /* Override ISocket Methods */
                virtual int setAttribute(int attrId, void* attr, int attrSize) override;
                virtual int getAttribute(int attrId, void* attr, int attrSize) override;
            protected:
                int initSocket();
                int destroySocket();
                struct AttributeBufferSize m_bufferSize;
                enum Type{
                    Type_INET = 0,
                    Type_UNIX
                };
                enum Type m_type;
            private:
                virtual int setBufferSize(int recvBufferSizeInByte, int sendBufferSizeInByte);
                virtual int getBufferSize(int *recvBufferSizeInByte, int *sendBufferSizeInByte);
            };
        }
    }
}


#endif // TCPSOCKET_H
/**
 * @}
 */ 
