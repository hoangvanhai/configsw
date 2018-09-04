/**
 * @addtogroup communication_endpoint
 * @{
 */ 
#ifndef __CONNECTION_TAP_SOCKET_H__
#define __CONNECTION_TAP_SOCKET_H__

#include "../ISocket.h"
#include <string>

namespace communication{
    namespace endpoint{
        namespace tap{
            /**
             * @brief Attributes
             */ 
            enum Attribute_ID{
                Attribute_PortName = 0, ///< TAP network interface name
            };
            /**
             * @brief Attributes for Attribute_PortName
             */ 
            struct AttributePortName{
                std::string portname;   ///< Network interface name
            };
            /**
             * @brief Implement TAP socket
             */ 
            class TAPSocket : public ISocket{
            public:
                TAPSocket();
                virtual ~TAPSocket();
                
                /* Override ISocket */
                virtual int writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj);
                virtual int readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj);
                virtual int setAttribute(int attrId, void* attr, int attrSize);
                virtual int getAttribute(int attrId, void* attr, int attrSize);
                
            private:
                std::string m_portName;
                int setPortName(struct AttributePortName *port);
                int getPortName(struct AttributePortName *port);
            };
        }
    }
}
#endif // UDPSOCKET_H
/**
 * @}
 */ 
