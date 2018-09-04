/**
 * @addtogroup communication_endpoint
 * @{
 */ 
#ifndef ETHERNETSOCKET_H
#define ETHERNETSOCKET_H

#include "../ISocket.h"

#include <string>
#include <functional>

#ifdef __linux__
#include <linux/if_packet.h>
#include <net/if.h>
#endif

namespace communication{
    namespace endpoint{
        namespace ethernet{
            /**
            * @brief Attribute ID
            */ 
            enum Attribute_ID{
                Attribute_Interface = 0,    ///< Get/Set ethernet interface
                Attribute_BufferSize,       ///< Get/Set socket buffer size
            };
            /**
            * @brief Attribute_Interface parameter
            */ 
            struct AttributeInterface{
                std::string ifname;         ///< Ethernet interface name
            };
            /**
            * @brief Attribute_BufferSize parameter
            */ 
            struct AttributeBufferSize{
                int recvBufferSize;         ///< Received buffer size in bytes
                int sendBufferSize;         ///< Send buffer size in bytes
                
                int blockSize;              ///< Received block size in bytes, eg: 65536
            };
            /**
            * @brief Object contains received callback
            */ 
            class ReceivedDataObject : public Object{
            public:
                ReceivedDataObject();
                virtual ~ReceivedDataObject();
                /**
                * @brief Socket will be call this function when it has ethernet frames need to read
                * @param frame Ethernet frame, begin by struct ether_header
                * @param frameLength Ethernet frame size in bytes
                */
                std::function<void(void* frame, int frameLength)> receivedEthernetFrame;
            };
            /**
            * @brief Raw-Ethernet socket
            * @note Implemented by mmap-packet
            * @note Must be has ROOT privilege to access I/O functions
            */ 
            class Socket : public ISocket{
            public:
                Socket();
                virtual ~Socket();
                
                /* Override ISocket */
                int waitEvent(int eventMustToWait, int timeoutInMilisecond);
                int writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object>& obj );
                /**
                * @param obj ReceivedDataObject
                * @note Ignore 'data' and 'dataSizeInByte' parameters, ReceivedDataObject will be called if has pending data frames
                */ 
                int readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj);
                int setAttribute(int attrId, void* attr, int attrSize);
                int getAttribute(int attrId, void* attr, int attrSize);

                static void getMacAddress(const char* ifname, uint8_t* addr);
            private:
                std::string m_ifname;
#ifdef __linux__
                /* MMAP Packet */
                struct iovec        *m_rd;
                uint8_t             *m_map;
                struct tpacket_req3  m_req;
                int                  m_blockIndex;
                int                  m_blockNum;
                struct ifreq         m_if_idx;
                struct ifreq         m_if_mac;
#elif defined(_MSC_VER)
                void*       m_handle;
                uint8_t     m_saddr[6];
#endif

                int setInterface(std::string &ifname);
                std::string getInterface();
                int setBufferSize(int rxBufferSize, int txBufferSize, int blockSize);
                int getBufferSize(int *rxBufferSize, int *txBufferSize, int *blockSize);
            };
        }
    }
}
#endif // ETHERNETSOCKET_H
/**
 * @}
 */ 
