#include "../../../cppframework.h"

#if defined(_MSC_VER)
#include "EthernetSocket.h"
#include "../SocketUtils.h"
#include "../../../safe/Safe.h"
#include <cstring>
using namespace communication;
using namespace communication::endpoint::ethernet;

#if COMMUNICATION_SOCKET_ETHERNET_ENABLED == 1
#include <Windows.h>
#include <IPHlpApi.h>
#pragma comment(lib, "iphlpapi.lib")
#if COMMUNICATION_SOCKET_ETHERNET_LIB == COMMUNICATION_SOCKET_ETHERNET_LIB_PCAP
#include <pcap/pcap.h>
#pragma comment(lib, "Packet.lib")
#pragma comment(lib, "wpcap.lib")
Socket::Socket(){
    m_handle = 0;
}
Socket::~Socket(){
    if (m_handle){
        pcap_close((pcap_t*)m_handle);
        m_handle = 0;
    }
}
int Socket::waitEvent(int eventMustToWait, int timeoutInMilisecond){
    return eventMustToWait;
}
int Socket::writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int wlen = 0;

    struct ethhdr{
        u_char ether_dhost[6];
        u_char ether_shost[6];
        u_short ether_type;
    };

    if (!m_handle) return wlen;
    struct ethhdr *eh = (struct ethhdr *) data;

    /* Ethernet header */
    for (int i = 0; i < 6; i++)
        eh->ether_shost[i] = m_saddr[i];
    // Destination address and ether type are assigned by upper layer
    //    eh->ether_dhost[0] = MY_DEST_MAC0;
    //    eh->ether_dhost[1] = MY_DEST_MAC1;
    //    eh->ether_dhost[2] = MY_DEST_MAC2;
    //    eh->ether_dhost[3] = MY_DEST_MAC3;
    //    eh->ether_dhost[4] = MY_DEST_MAC4;
    //    eh->ether_dhost[5] = MY_DEST_MAC5;
    /* Ethertype field */
    //    eh->ether_type = htons(ETH_P_IP);
    wlen = pcap_sendpacket((pcap_t*)m_handle, (const u_char*)data, dataSizeInByte);
    return wlen;
}
int Socket::readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int err = 0;
    int rlen = 0;

    if (!m_handle) return rlen;
    std::shared_ptr<ReceivedDataObject> recvObj = std::dynamic_pointer_cast<ReceivedDataObject>(obj);
    if (recvObj == nullptr) return rlen;

    struct pcap_pkthdr *header = 0;
    const u_char *pkt_data = 0;

    struct ethhdr{
        u_char ether_dhost[6];
        u_char ether_shost[6];
        u_short ether_type;
    };

    err = pcap_next_ex((pcap_t*)m_handle, &header, &pkt_data);
    if (err > 0){
        struct ethhdr* eth = (struct ethhdr*)pkt_data;
        if ((eth->ether_shost[0] == m_saddr[0]) &&
            (eth->ether_shost[1] == m_saddr[1]) &&
            (eth->ether_shost[2] == m_saddr[2]) &&
            (eth->ether_shost[3] == m_saddr[3]) &&
            (eth->ether_shost[4] == m_saddr[4]) &&
            (eth->ether_shost[5] == m_saddr[5]))
            return rlen;
        recvObj->receivedEthernetFrame((void*)pkt_data, header->len);
        rlen += header->len;
    }

    return rlen;
}
int Socket::setInterface(std::string &ifname){
    int err = -1;
    pcap_if_t *alldevs = 0;
    pcap_if_t *d = 0;
    uint8_t errbuf[PCAP_ERRBUF_SIZE];
    bool found = false;

    /* Retrieve the device list */
    if (pcap_findalldevs(&alldevs, (char*)errbuf) == -1){
        return err;
    }
    for (d = alldevs; d; d = d->next){
        if (d->name){
            if (std::string(d->name) == ifname){
                err = 0;
                break;
            }
        }
    }
    if (!err){
        err = -1;
        /* Open the adapter */
        m_handle = pcap_open_live(d->name,    // name of the device
            65536,            // portion of the packet to capture. 
            // 65536 grants that the whole packet will be captured on all the MACs.
            1,                // promiscuous mode (nonzero means promiscuous)
            1000,            // read timeout
            (char*)errbuf            // error buffer
            );
        if (m_handle){
            getMacAddress(d->name, m_saddr);
            m_ifname = ifname;
            err = 0;
        }
    }
    /* Free the device list */
    pcap_freealldevs(alldevs);
    return err;
}
int Socket::setBufferSize(int rxBufferSize, int txBufferSize, int blockSize){
    return 0;
}
int Socket::getBufferSize(int *rxBufferSize, int *txBufferSize, int *blockSize){
    return 0;
}
void Socket::getMacAddress(const char* ifname, uint8_t* addr){
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT i;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo){
        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW){
            free(pAdapterInfo);
            pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
        }
        if (pAdapterInfo){
            if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR){
                pAdapter = pAdapterInfo;
                std::string srcName = std::string(ifname);
                while (pAdapter){
                    std::string aname = std::string(pAdapter->AdapterName);
                    if (srcName.find(aname) != std::string::npos){
                        for (int i = 0; i < 6; i++)
                            addr[i] = pAdapter->Address[i];
                        break;
                    }
                    pAdapter = pAdapter->Next;
                }
            }
            free(pAdapterInfo);
        }
    }
}
#endif // COMMUNICATION_SOCKET_ETHERNET_LIB == "pcap"
#endif // COMMUNICATION_SOCKET_ETHERNET_ENABLED == 1
#endif // _MSC_VER

