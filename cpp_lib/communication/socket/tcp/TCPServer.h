/**
 * @addtogroup communication_endpoint
 * @{
 */
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include "TCPSocket.h"

namespace communication{
    namespace endpoint{
        namespace tcp{
            /**
             * @brief Implement TCP socket as Server
            *  Attributes
                @code
                +-----------------------------------+----------------------------------------+------------+---------------------------------+---------+
                |             ID                     |           Attribute                   |   Length   |            Desc                 | Action  |
                +------------------------------------+---------------------------------------+------------+---------------------------------+---------+
                | Attribute_ListenAddressAndPort     | struct AttributeListenAddressAndPort& |   ignore   | setup listen address and port   | Get/Set |
                | Attribute_ListenQueueSize          | int&                                  |   ignore   | setup max count client in queue | Get/Set |
                +------------------------------------+---------------------------------------+------------+---------------------------------+---------+
                @endcode
            */ 
            class Server : public TCPSocket, public IServer{
            public:
                /**
                 * @brief Attributes
                 */ 
                enum Attribute_TCP_SERVER_ID{
                    Attribute_ListenAddressAndPort = Attribute_TCP_SOCKET_ID_END + 1,   ///< Listen address and port
                    Attribute_ListenQueueSize,                                          ///< Listen queue size, attr=&int
                    Attribute_TCP_SERVER_ID_END
                };
                /**
                 * @brief Attributes for Attribute_ListenAddressAndPort
                 */ 
                struct AttributeListenAddressAndPort{
                    std::string address;        ///< Listen address
                    int port;                   ///< Listen port
                };
                Server();
                virtual ~Server();
                
                /* Override ISocket Methods */
                virtual int writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj) override;
                virtual int readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj) override;
                virtual int setAttribute(int attrId, void* attr, int attrSize) override;
                virtual int getAttribute(int attrId, void* attr, int attrSize) override;
                /* Override IServer Methods */
                virtual int startListen() override;
                virtual std::shared_ptr<communication::ISocket> acceptClient() override;
            private:
                struct AttributeListenAddressAndPort m_listenAddr;
                int m_listenQueueSize;

                int setDestination(struct AttributeListenAddressAndPort* attr);
                int getDestination(struct AttributeListenAddressAndPort* attr);
                int setListenQueueSize(int size);
                int getListenQueueSize(int* size);            
                virtual std::shared_ptr<communication::ISocket> makeClientInstance();
            };
        }
    }
}

#endif
/**
 * @}
 */ 
