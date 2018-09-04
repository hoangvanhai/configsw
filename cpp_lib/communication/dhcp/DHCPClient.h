/**
 * @addtogroup communication
 * @{
 * @defgroup communication_protocol_dhcp Implement DHCP protocol over raw ethernet socket
 * @{
 *
 */ 
#ifndef DHCPCLIENT_H
#define DHCPCLIENT_H

#include "../socket/ISocket.h"

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace communication{
    namespace protocol{
        namespace dhcp{
            /**
             * @brief DHCP information
             */ 
            struct DHCP{
                std::string address;        ///< IP address
                std::string netmask;        ///< Netmask
                
                std::string routeAddress;   ///< Route's IP address
                std::string serverAddress;  ///< DHCP server's IP address
                std::string dnsAddress[2];  ///< DNS IP address
                int         leaseTime;      ///< Lease time in milisecond
                std::string domainName;     ///< Domain name
                
                /**
                 * @brief Reset all parameters to default
                 */ 
                void reset();
            };
            /**
             * @brief Client connect to DHCP server and implemented DHCP protocol
             * @note Must has ROOT privilege for access RAW ETHERNET socket
             */ 
            class Client{
            public:
                Client();
                virtual ~Client();
                /**
                 * @brief Set Network Interface for exchange DHCP messages
                 * @param ifname Network Interface, eg: eth0
                 * @retval 0 Successful
                 */ 
                int setInterface(std::string ifname);
                /**
                 * @brief Set DHCP server and local listening UDP port
                 * @param serverPort Server listening port
                 * @param clientPort Client listening port
                 * @retval 0 Successful
                 */ 
                int setPort(int serverPort, int clientPort);
                /**
                 * @brief Start and run DHCP request flow
                 * @param callback Callback called when DONE, dhcp: DHCP information is received from DHCP server
                 * @param waitDone Block this call until end of flow
                 * @param timeoutInMilisecond Timeout if block is enabled in milisecond
                 * @retval 0 Successful
                 */ 
                int run(
                    std::function<int(struct DHCP &dhcp)> callback,
                    bool waitDone = true,
                    int timeoutInMilisecond = 5000
                       );
                /**
                 * @brief Break and stop flow
                 * @retval 0 Successful
                 */ 
                int stop();     
            private:
                std::string m_ifname;
                std::thread m_thread;
                volatile bool m_term;
                int m_serverPort;
                int m_clientPort;
                std::shared_ptr<ISocket> m_sock;
                uint32_t m_xid;
                
                int initSocket();
                int sendDiscovery();
                int sendRequest(struct DHCP &dhcp);
                int sendDHCPPacket(void* data, int dataLength, void* mac);
                uint16_t in_cksum(unsigned short *addr, int len);
                int getDHCPType(void* data, int len);
                int extractDHCPPacket(void* pkg, int len, struct DHCP &dhcp);
            };
        }
    }
}

#endif // DHCPCLIENT_H
/** @} @}
 */ 
