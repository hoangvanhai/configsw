/**
 * @addtogroup communication_endpoint
 * @{
 */ 
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include "TCPSocket.h"

namespace communication{
    namespace endpoint{
        namespace tcp{
            /**
             * @brief Implement TCP socket as Client
            *  Attributes
                @code
                +---------------------------+------------------------------+------------+---------------------------------+---------+
                |             ID            |           Attribute          |   Length   |            Desc                 | Action  |
                +---------------------------+------------------------------+------------+---------------------------------+---------+
                | Attribute_Destination     | struct AttributeDestination& |   ignore   | setup server address and port   | Get/Set |
                +---------------------------+------------------------------+------------+---------------------------------+---------+
                @endcode
            */ 
            class Client : public TCPSocket, public communication::IClient{
            public:
                /**
                 * @brief Attributes
                 */ 
                enum Attribute_CLIENT_ID{
                    Attribute_Destination = Attribute_TCP_SOCKET_ID_END + 1,    ///< Connection's destination address
                    Attribute_CLIENT_TCP_SOCKET_ID_END
                };
                /**
                 * @brief Atributes for Attributes_Destination
                 */ 
                struct AttributeDestination{
                    std::string address;    ///< Server's IP address
                    int port;               ///< Server's listen port
                };
                Client();
                virtual ~Client();
                
                /* Override ISocket Methods */
                virtual int writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj) override;
                virtual int readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj) override;
                virtual int setAttribute(int attrId, void* attr, int attrSize) override;
                virtual int getAttribute(int attrId, void* attr, int attrSize) override;
                /* Override IClient Methods */
                virtual int connectToServer(int waitTimeoutInMilisecond) override;
                virtual int disconnectToServer() override;
            private:
                struct AttributeDestination m_destination;
                int setDestination(struct AttributeDestination* attr);
                int getDestination(struct AttributeDestination* attr);
            };
        }
    }
}

#endif
/**
 * @}
 */ 
