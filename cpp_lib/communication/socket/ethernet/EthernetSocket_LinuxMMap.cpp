#include "../../../cppframework.h"

#ifdef __linux__
#include "EthernetSocket.h"
#include "../SocketUtils.h"
#include "../../../safe/Safe.h"
#include <cstring>

using namespace communication;
using namespace communication::endpoint::ethernet;

#include <net/if.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <sys/ioctl.h>

Socket::Socket():
m_ifname("lo")
{    
    m_sockID = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(m_sockID != SOCKET_INVALID){
        int v = TPACKET_V3;
        setsockopt(m_sockID, SOL_PACKET, PACKET_VERSION, &v, sizeof(v));
    }
    
    m_rd  = 0;
    m_map = (uint8_t*)MAP_FAILED;
    m_blockIndex = 0;
    m_blockNum   = 0;
}
Socket::~Socket(){
}
int Socket::waitEvent(int eventMustToWait, int timeoutInMilisecond){
    int event = Event_Error;
    
    struct pollfd pfd;

    memset(&pfd, 0, sizeof(pfd));
    pfd.fd      = m_sockID;
    pfd.events  = POLLERR;
    pfd.revents = 0;
    
    if(eventMustToWait & Event_Readable) pfd.events |= POLLIN;
    if(eventMustToWait & Event_Writable) pfd.events |= POLLOUT;

    int err = poll(&pfd, 1, timeoutInMilisecond);
    if(err == 0){
        event = Event_Timeout;
    }else if(err > 0){
        event = 0;
        if(pfd.revents & POLLIN){
            event |= Event_Readable;
        }
        if(pfd.revents & POLLOUT){
            event |= Event_Writable;
        }
        if(pfd.revents & POLLERR){
            event |= Event_Error;
        }
    }else{
        event = Event_Error;
    }
    
    return event;
}
int Socket::writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object>& obj){
    int wlen = 0;
    
    if(m_sockID == SOCKET_INVALID) return wlen;
    
    struct ethhdr *eh = (struct ethhdr *) data;
    struct sockaddr_ll socket_address;

    memset(&socket_address, 0, sizeof(socket_address));
    /* Ethernet header */
    cppframework::safe::s_memcpy(eh->h_source, m_if_mac.ifr_hwaddr.sa_data, ETH_ALEN, ETH_ALEN);
    // Destination address and ether type are assigned by upper layer
//    eh->ether_dhost[0] = MY_DEST_MAC0;
//    eh->ether_dhost[1] = MY_DEST_MAC1;
//    eh->ether_dhost[2] = MY_DEST_MAC2;
//    eh->ether_dhost[3] = MY_DEST_MAC3;
//    eh->ether_dhost[4] = MY_DEST_MAC4;
//    eh->ether_dhost[5] = MY_DEST_MAC5;
    /* Ethertype field */
//    eh->ether_type = htons(ETH_P_IP);
    socket_address.sll_ifindex = m_if_idx.ifr_ifindex;
    socket_address.sll_family  = AF_PACKET;
    socket_address.sll_halen   = ETH_ALEN;
    cppframework::safe::s_memcpy(socket_address.sll_addr, eh->h_source, ETH_ALEN, ETH_ALEN);

    wlen = sendto(m_sockID, data, dataSizeInByte, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll));  

    return wlen;
}
int Socket::readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int rlen = 0;
    
    if(m_sockID == SOCKET_INVALID) return rlen;
    
    std::shared_ptr<ReceivedDataObject> recvObj = std::dynamic_pointer_cast<ReceivedDataObject>(obj);
    if(recvObj == nullptr) return rlen;
    
    typedef struct __attribute__((packed)) {
        uint32_t version;
        uint32_t offset_to_priv;
        struct tpacket_hdr_v1 h1;
    }BLOCK_DESC;

    BLOCK_DESC *pbd = (BLOCK_DESC*)m_rd[m_blockIndex].iov_base;

    if((pbd->h1.block_status & TP_STATUS_USER) != 0) {
        int num_pkts = pbd->h1.num_pkts;
        struct tpacket3_hdr *ppd;

        ppd = (struct tpacket3_hdr *) ((uint8_t *) pbd + pbd->h1.offset_to_first_pkt);
        for (int i = 0; i < num_pkts; ++i) {
            recvObj->receivedEthernetFrame(((uint8_t *) ppd + ppd->tp_mac), ppd->tp_snaplen);
            rlen += ppd->tp_snaplen;
            ppd = (struct tpacket3_hdr *) ((uint8_t *) ppd + ppd->tp_next_offset);
        }
        pbd->h1.block_status = TP_STATUS_KERNEL;
    }
    m_blockIndex = (m_blockIndex + 1) % m_blockNum;
    
    return rlen;
}
int Socket::setInterface(std::string &ifname){
    int err = -1;
    if(m_sockID == SOCKET_INVALID) return err;
    memset(&m_if_idx, 0, sizeof(struct ifreq));
    cppframework::safe::s_memcpy(m_if_idx.ifr_name, ifname.c_str(), ifname.length(), IFNAMSIZ);
    if (ioctl(m_sockID, SIOCGIFINDEX, &m_if_idx) < 0){
        err = -2;
    }else{
        // Bind
        struct sockaddr_ll sll;
        memset(&sll, 0, sizeof(sll));
        sll.sll_family  = AF_PACKET;
        sll.sll_ifindex = m_if_idx.ifr_ifindex;
        sll.sll_protocol= htons(ETH_P_ALL);
        bind(m_sockID, (struct sockaddr*)&sll, sizeof(sll));
        
        /* Get the MAC address of the interface to send on */
        memset(&m_if_mac, 0, sizeof(struct ifreq));
        cppframework::safe::s_memcpy(m_if_mac.ifr_name, ifname.c_str(), ifname.length(), IFNAMSIZ);
        if (ioctl(m_sockID, SIOCGIFHWADDR, &m_if_mac) < 0){
            err = -3;
        }else{
            m_ifname = ifname;
            err = 0;
        }
    }
    
    return err;
}
int Socket::setBufferSize(int rxBufferSize, int txBufferSize, int blockSize){
    int err = -1;
    
    if(m_sockID == SOCKET_INVALID) return err;
    /* note
     *  After set tx buffer size
     *      When tx buffer is full, writeData will be return (-1)
     *      and errno = 105 (No buffer space available) 
     *      but waitEvent return Event_Writable => FAIL
     *  If not set tx buffer size
     *      waitEvent will be blocked until tx buffer is available => CORRECT
     */
//     communication::utils::Network::setBufferSize(this, 0, txBufferSize);
    
    int framesiz = getpagesize();
    int blocksiz = blockSize;
    if(blocksiz < framesiz) blocksiz = framesiz;
    int blocknum = rxBufferSize / blockSize;
    
    if(blocknum < 1) blocknum = 1;
    
    // RX buffer size
    memset(&m_req, 0, sizeof(m_req));
    m_req.tp_block_size = blocksiz;
    m_req.tp_frame_size = framesiz;
    m_req.tp_block_nr   = blocknum;
    m_req.tp_frame_nr   = (blocksiz * blocknum) / framesiz;
    m_req.tp_retire_blk_tov = 60;
    m_req.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH;
    
    err = setsockopt(m_sockID, SOL_PACKET, PACKET_RX_RING, &m_req, sizeof(m_req));
    if (err >= 0) {
        m_map = (uint8_t*)mmap(NULL, m_req.tp_block_size * m_req.tp_block_nr,
                    PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, m_sockID, 0);
        if (m_map != MAP_FAILED) {
            m_rd = (struct iovec*)malloc(m_req.tp_block_nr * sizeof(struct iovec));
            struct iovec* rd = m_rd;
            for (unsigned int i = 0; i < m_req.tp_block_nr; ++i) {
                rd->iov_base = m_map + (i * m_req.tp_block_size);
                rd->iov_len = m_req.tp_block_size;
                rd++;
            }
            m_blockIndex = 0;
            m_blockNum   = blocknum;
            err = 0;
        }
    }
    
    return err;
}
int Socket::getBufferSize(int *rxBufferSize, int *txBufferSize, int *blockSize){
    int err = -1;
    if(m_sockID == SOCKET_INVALID) return err;
    
    *rxBufferSize = m_req.tp_block_nr * m_req.tp_block_size;
    *blockSize    = m_req.tp_block_size;
        
    int rx; int tx;
    communication::utils::Network::getBufferSize(this, &rx, &tx);
    *txBufferSize = tx;
    
    err = 0;
    
    return err;
}
void Socket::getMacAddress(const char* ifname, uint8_t* addr){
    struct ifreq ifr;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    int result;
    memset(&ifr, 0, sizeof(ifr));
    
    for(int i = 0; (i < IFNAMSIZ) && ifname[i] != '\0'; i++){
        ifr.ifr_name[i] = ifname[i];
    }

//     strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    result = ioctl(fd, SIOCGIFHWADDR, &ifr);
    close(fd);
    if (result != 0)
        return;
    memmove((void *)addr, ifr.ifr_addr.sa_data, 6);
}
#endif // __linux__
