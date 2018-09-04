
#ifndef __DHCP_PROTOCOL_H__
#define __DHCP_PROTOCOL_H__

#include <cstdint>

namespace communication{
    namespace protocol{
        namespace dhcp{
        
typedef struct{
    uint8_t    opcode;
    uint8_t    htype;
    uint8_t    hlen;
    uint8_t    hops;
    uint32_t   xid;
    uint16_t   secs;
    uint16_t   flags;
    uint32_t   ciaddr;
    uint32_t   yiaddr;
    uint32_t   siaddr;
    uint32_t   giaddr;
    uint8_t    chaddr[16];
    int8_t     bp_sname[64];
    int8_t     bp_file[128];
    uint32_t   magic_cookie;
    uint8_t    bp_options[0];
}Packet;

enum CONST{ 
    DHCP_SERVER_PORT    = 67,
    DHCP_CLIENT_PORT    = 68,

    DHCP_MAGIC_COOKIE   = 0x63825363,

    DHCP_BOOTREQUEST                    = 1,
    DHCP_BOOTREPLY                      = 2,

    DHCP_HARDWARE_TYPE_10_EHTHERNET     = 1,

    MESSAGE_TYPE_PAD                    = 0,
    MESSAGE_TYPE_REQ_SUBNET_MASK        = 1,
    MESSAGE_TYPE_ROUTER                 = 3,
    MESSAGE_TYPE_DNS                    = 6,
    MESSAGE_TYPE_DOMAIN_NAME            = 15,
    MESSAGE_TYPE_REQ_IP                 = 50,
    MESSAGE_TYPE_LEASE_TIME             = 51,
    MESSAGE_TYPE_DHCP                   = 53,
    MESSAGE_TYPE_DHCP_SERVER_ID         = 54,
    MESSAGE_TYPE_PARAMETER_REQ_LIST     = 55,
    MESSAGE_TYPE_END                    = 255,

    DHCP_OPTION_DISCOVER                = 1,
    DHCP_OPTION_OFFER                   = 2,
    DHCP_OPTION_REQUEST                 = 3,
    DHCP_OPTION_PACK                    = 4,
    DHCP_OPTION_ACK                     = 5,
    DHCP_OPTION_NACK                    = 6,
};
        
}}}


#if defined(__linux__)
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#elif defined(_MSC_VER) // __linux__
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma pack(push, 1)
struct ether_header{
    uint8_t ether_dhost[6];
    uint8_t ether_shost[6];
    uint16_t ether_type;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct
    ip{
    uint8_t ip_hl : 4,
            ip_v : 4;
    uint8_t ip_tos;
    uint16_t ip_len;
    uint16_t ip_id;
    uint16_t ip_off;
    uint8_t ip_ttl;
    uint8_t ip_p;
    uint16_t ip_sum;
    struct  in_addr ip_src, ip_dst;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct udphdr{
    uint16_t source;
    uint16_t dest;
    uint16_t len;
    uint16_t check;
};
#pragma pack(pop)

typedef uint32_t u_int32_t;
typedef uint8_t u_int8_t;
#define ETHERTYPE_IP 0x0800
#define IPVERSION     4
#endif // _MSC_VER

#endif
