#include "DHCPClient.h"
#include "DHCPProtocol.h"

#include "../socket/ethernet/EthernetSocket.h"
#include "../../dll/dll_function.h"
#include "../../safe/Safe.h"
#include "../../string/StringUtils.h"
#include "../../rand/Random.h"
#include <cstring>

#include <iostream>
#include <chrono>

using namespace communication::protocol::dhcp;

void DHCP::reset(){
    address = "";
    netmask = "";
    routeAddress = "";
    serverAddress= "";
    dnsAddress[0] = "";
    dnsAddress[1] = "";
    leaseTime = 0;
    domainName = "";
}

Client::Client():
m_ifname("lo")
{
    m_term   = true;
    m_serverPort = DHCP_SERVER_PORT;
    m_clientPort = DHCP_CLIENT_PORT;
    m_xid        = 0;
}
Client::~Client(){
    stop();
}
int Client::setInterface(std::string ifname){
    m_ifname = ifname;
    return 0;
}
int Client::setPort(int serverPort, int clientPort){
    m_serverPort = serverPort;
    m_clientPort = clientPort;
    return 0;
}
int Client::extractDHCPPacket(void* data, int len, struct DHCP &dhcp){
    Packet* pkg = static_cast<Packet*>(data);
    
    uint8_t* opt = pkg->bp_options;
    len = len - sizeof(Packet);
        
    uint32_t yiaddr = ntohl(pkg->yiaddr);
    uint32_t siaddr = ntohl(pkg->siaddr);
    
	dhcp.address = cppframework::str::Util::format("{}.{}.{}.{}", yiaddr >> 24, ((yiaddr << 8) >> 24), (yiaddr << 16) >> 24, (yiaddr << 24) >> 24);
	dhcp.serverAddress = cppframework::str::Util::format("{}.{}.{}.{}", siaddr >> 24, ((siaddr << 8) >> 24), (siaddr << 16) >> 24, (siaddr << 24) >> 24);
        
    while(len > 0){
        uint8_t code = opt[0];
        uint8_t dlen = opt[1];
        opt += 2;
        
        if(code == MESSAGE_TYPE_REQ_SUBNET_MASK){
             uint32_t val = ntohl(*((uint32_t*)opt));
			 dhcp.netmask = cppframework::str::Util::format("{}.{}.{}.{}", val >> 24, ((val << 8) >> 24), (val << 16) >> 24, (val << 24) >> 24);
        }else if(code == MESSAGE_TYPE_ROUTER){
             uint32_t val = ntohl(*((uint32_t*)opt));
			dhcp.routeAddress = cppframework::str::Util::format("{}.{}.{}.{}", val >> 24, ((val << 8) >> 24), (val << 16) >> 24, (val << 24) >> 24);
        }else if(code == MESSAGE_TYPE_DHCP_SERVER_ID){
             uint32_t val = ntohl(*((uint32_t*)opt));
			dhcp.serverAddress = cppframework::str::Util::format("{}.{}.{}.{}", val >> 24, ((val << 8) >> 24), (val << 16) >> 24, (val << 24) >> 24);
        }else if(code == MESSAGE_TYPE_DNS){
             uint32_t val = ntohl(*((uint32_t*)opt));
			 dhcp.dnsAddress[0] = cppframework::str::Util::format("{}.{}.{}.{}", val >> 24, ((val << 8) >> 24), (val << 16) >> 24, (val << 24) >> 24);
            if(dlen >= 8){
                val = ntohl(*(((uint32_t*)opt)+1));
				dhcp.dnsAddress[1] = cppframework::str::Util::format("{}.{}.{}.{}", val >> 24, ((val << 8) >> 24), (val << 16) >> 24, (val << 24) >> 24);
            }
        }else if(code == MESSAGE_TYPE_LEASE_TIME){
            uint32_t val = ntohl(*((uint32_t*)opt));
            dhcp.leaseTime = val;
        }else if(code == MESSAGE_TYPE_DOMAIN_NAME){
            int8_t buf[1024];
            memset(buf, 0, 1024);
            int clen = dlen;
            if(clen > 1023) clen = 1023;
            cppframework::safe::s_memcpy(buf, opt, clen, 1024);
            dhcp.domainName = std::string((const char*)buf);
        }
        
        opt += dlen;
        len -= (2 + dlen);        
    }
    
    
    return 0;
}
int Client::sendRequest(struct DHCP &dhcp){
    int err = -1;
    uint8_t txbuf[1024];
    uint8_t mac[6];
    
    // prepare
    memset(txbuf, 0, 1024);
    memset(mac, 0, 6);
    // get mac of interface
	communication::endpoint::ethernet::Socket::getMacAddress((const char*)m_ifname.c_str(), mac);
    Packet* pkg = (Packet*)(&txbuf[sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr)]);
    
    // Packet
    auto buildOption = [](uint8_t* pkg, uint8_t code, uint8_t* data, uint8_t len){
        pkg[0] = code;
        pkg[1] = len;
        cppframework::safe::s_memcpy(&pkg[2], data, len, len);
        return len + 2;
    };
    int len = 0;
    u_int32_t ip_val;
    u_int8_t option;
    struct in_addr inaddr;
    
    option = DHCP_OPTION_REQUEST;
    len += buildOption(&pkg->bp_options[len], MESSAGE_TYPE_DHCP, &option, sizeof(option));
    
    inet_pton(AF_INET, dhcp.address.c_str(), &inaddr);
    ip_val = inaddr.s_addr;
    len += buildOption(&pkg->bp_options[len], MESSAGE_TYPE_REQ_IP, (u_int8_t *)&ip_val, sizeof(ip_val));
    inet_pton(AF_INET, dhcp.serverAddress.c_str(), &inaddr);
    ip_val = inaddr.s_addr;
    len += buildOption(&pkg->bp_options[len], MESSAGE_TYPE_DHCP_SERVER_ID, (u_int8_t *)&ip_val, sizeof(ip_val));
    option = 0;
    len += buildOption(&pkg->bp_options[len], MESSAGE_TYPE_END, &option, sizeof(option));
    
    len += sizeof(Packet);
    pkg->opcode = DHCP_BOOTREQUEST;
    pkg->htype  = DHCP_HARDWARE_TYPE_10_EHTHERNET;
    pkg->hlen   = 6;
    cppframework::safe::s_memcpy(pkg->chaddr, mac, 6, 6);
    pkg->magic_cookie = htonl(DHCP_MAGIC_COOKIE);
    pkg->xid    = htonl(m_xid);
    
    int wlen = sendDHCPPacket(txbuf, len, mac);
    if(wlen > 0) err = 0;
        
    return err;
}
int Client::getDHCPType(void* data, int len){
    int ret = 0;
    Packet* pkg = static_cast<Packet*>(data);
    
    uint8_t* opt = pkg->bp_options;
    len = len - sizeof(Packet);
        
    while(len > 0){
        uint8_t code = opt[0];
        uint8_t dlen = opt[1];
        opt += 2;
        
        if(code == MESSAGE_TYPE_DHCP){
             ret = opt[0];
             break;
        }
        opt += dlen;
        len -= (2 + dlen);        
    }
    
    return ret;
}
int Client::run(
    std::function<int(struct DHCP &dhcp)> callback,
    bool waitDone,
    int timeoutInMilisecond
               )
{
    struct DHCP ret;  
    
    ret.reset();

    stop();

    m_xid = ((uint32_t)(cppframework::rand::Random::generate(0, 255) << 0)) | 
            ((uint32_t)(cppframework::rand::Random::generate(0, 255) << 8)) |
            ((uint32_t)(cppframework::rand::Random::generate(0, 255) << 16)) |
            ((uint32_t)(cppframework::rand::Random::generate(0, 255) << 24));
    
    int err = initSocket();
    
    if(err){
        stop();
        return err;
    }
    err = -1;
    
    auto processDHCPPacket = [&](void* data, int len){
        Packet* pkg = static_cast<Packet*>(data);
        uint32_t xid = ntohl(pkg->xid);
        if(xid != m_xid) return;
        
        if(pkg->opcode == DHCP_BOOTREPLY){
            int type = getDHCPType(data, len);
            if(type == DHCP_OPTION_OFFER){
                extractDHCPPacket(pkg, len, ret);
                sendRequest(ret);
            }else if(type == DHCP_OPTION_ACK){
                extractDHCPPacket(pkg, len, ret);
                err    = 0;
                m_term = true;
                callback(ret);
            }else if(type == DHCP_OPTION_NACK){
                m_term = true;
            }
        }
    };
    
    std::shared_ptr<communication::endpoint::ethernet::ReceivedDataObject> recvObj = std::make_shared<communication::endpoint::ethernet::ReceivedDataObject>();
    recvObj->receivedEthernetFrame = [&](void* frame, int frameSize){
        struct ether_header* eth_pkg = (struct ether_header*)frame;
//         std::cout<<"eth="<<ntohs(eth_pkg->ether_type)<<std::endl;
        if(ntohs(eth_pkg->ether_type) == ETHERTYPE_IP){
            struct ip *ip_pkg = (struct ip *)(&eth_pkg[1]);
//             std::cout<<"ip_pkg->ip_p="<<(int)ip_pkg->ip_p<<std::endl;
            if(ip_pkg->ip_p == IPPROTO_UDP){
                struct udphdr *udp_pkg = (struct udphdr *)(&ip_pkg[1]);
                if(ntohs(udp_pkg->source) == m_serverPort){
                    int len = frameSize - sizeof(struct ether_header) - sizeof(ip) - sizeof(udphdr);
                    if(len)
                        processDHCPPacket(&udp_pkg[1], len);
                }
            }
        }
    };
    
    volatile bool threadReady = false;
    m_term = false;
    m_thread = std::thread([&]{
        auto ts1 = std::chrono::high_resolution_clock::now();
        threadReady = true;
        while(!m_term){
            int event = m_sock->waitEvent(communication::Event_Readable, 100);
            if(event & communication::Event_Error){
                break;
            }
            if(event & communication::Event_Readable){
                m_sock->readData(0, 0, recvObj);
            }else if(event == communication::Event_Timeout){
            }
            if(waitDone){
                auto ts2 = std::chrono::high_resolution_clock::now();
				std::chrono::duration<int, std::milli> diff = std::chrono::duration_cast<std::chrono::milliseconds>(ts2 - ts1);
                if(diff.count() >= timeoutInMilisecond){
                    break;
                }
            }
        }
    });
    
    // send dhcp discovery
    if(!threadReady){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    sendDiscovery();
    
    if(waitDone){
        if(m_thread.joinable()) m_thread.join();
        stop();
    }
        
    return err;
    
}
int Client::stop(){
    m_term = true;
    if(m_thread.joinable()) m_thread.join();
    if(m_sock) m_sock.reset(); 
    m_sock = nullptr;
    return 0;
}
int Client::initSocket(){    
    m_sock = std::make_shared<communication::endpoint::ethernet::Socket>();
    
    // set interface
    communication::endpoint::ethernet::AttributeInterface ifname;
    ifname.ifname = m_ifname;
    int err = m_sock->setAttribute(communication::endpoint::ethernet::Attribute_Interface, &ifname);
    if(err){
        std::cout<<"set interface failed"<<std::endl;
        return err;
    }
    // set buffer size
    communication::endpoint::ethernet::AttributeBufferSize bufSize;
    bufSize.recvBufferSize = 1024*1024;
    bufSize.sendBufferSize = 1024*1024;
    bufSize.blockSize      = 65536;
    err = m_sock->setAttribute(communication::endpoint::ethernet::Attribute_BufferSize, &bufSize);
    
    return err;
}
int Client::sendDHCPPacket(void* data, int len, void* mac){
    
    // prepare
    
    // build packet    
    struct ether_header* eth_pkg = (struct ether_header*)data;
    struct ip *ip_pkg = (struct ip *)(&eth_pkg[1]);
    struct udphdr *udp_pkg = (struct udphdr *)(&ip_pkg[1]);
    
    // udp
    if(len & 1) len += 1;
    len += sizeof(struct udphdr);
    udp_pkg->source = htons(m_clientPort);
    udp_pkg->dest   = htons(m_serverPort);
    udp_pkg->len    = htons(len);
    udp_pkg->check  = 0;
    
    // ip
    len += sizeof(struct ip);
    ip_pkg->ip_hl = 5;
    ip_pkg->ip_v = IPVERSION;
    ip_pkg->ip_tos = 0x10;
    ip_pkg->ip_len = htons(len);
    ip_pkg->ip_id = htonl(0xffff);
    ip_pkg->ip_off = 0;
    ip_pkg->ip_ttl = 16;
    ip_pkg->ip_p = IPPROTO_UDP;
    ip_pkg->ip_sum = 0;
    ip_pkg->ip_src.s_addr = 0;
    ip_pkg->ip_dst.s_addr = 0xFFFFFFFF;
    ip_pkg->ip_sum = in_cksum((unsigned short *) ip_pkg, sizeof(struct ip));
    
    // ether
    cppframework::safe::s_memcpy(eth_pkg->ether_shost, mac, 6, 6);
    memset(eth_pkg->ether_dhost, 0xFF,  6);
    eth_pkg->ether_type = htons(ETHERTYPE_IP);

    len = len + sizeof(struct ether_header);
    
    int wlen = m_sock->writeData(data, len);
    
    return wlen;
}
int Client::sendDiscovery(){
    int err = -1;
    uint8_t txbuf[1024];
    uint8_t mac[6];
    
    // prepare
    memset(txbuf, 0, 1024);
    memset(mac, 0, 6);
    // get mac of interface
	communication::endpoint::ethernet::Socket::getMacAddress((const char*)m_ifname.c_str(), mac);
    Packet* pkg = (Packet*)(&txbuf[sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr)]);
    
    // Packet
    auto buildOption = [](uint8_t* pkg, uint8_t code, uint8_t* data, uint8_t len){
        pkg[0] = code;
        pkg[1] = len;
        cppframework::safe::s_memcpy(&pkg[2], data, len, len);
        return len + 2;
    };
    int len = 0;
    u_int32_t req_ip;
    u_int8_t parameter_req_list[] = {MESSAGE_TYPE_REQ_SUBNET_MASK, MESSAGE_TYPE_ROUTER, MESSAGE_TYPE_DNS, MESSAGE_TYPE_DOMAIN_NAME};
    u_int8_t option;
    option = DHCP_OPTION_DISCOVER;
    len += buildOption(&pkg->bp_options[len], MESSAGE_TYPE_DHCP, &option, sizeof(option));
    req_ip = htonl(0xc0a8010a);
    len += buildOption(&pkg->bp_options[len], MESSAGE_TYPE_REQ_IP, (u_int8_t *)&req_ip, sizeof(req_ip));
    len += buildOption(&pkg->bp_options[len], MESSAGE_TYPE_PARAMETER_REQ_LIST, (u_int8_t *)&parameter_req_list, sizeof(parameter_req_list));
    option = 0;
    len += buildOption(&pkg->bp_options[len], MESSAGE_TYPE_END, &option, sizeof(option));
    
    len += sizeof(Packet);
    pkg->opcode = DHCP_BOOTREQUEST;
    pkg->htype  = DHCP_HARDWARE_TYPE_10_EHTHERNET;
    pkg->hlen   = 6;
    cppframework::safe::s_memcpy(pkg->chaddr, mac, 6, 6);
    pkg->magic_cookie = htonl(DHCP_MAGIC_COOKIE);
    pkg->xid    = htonl(m_xid);
    
    int wlen = sendDHCPPacket(txbuf, len, mac);
    if(wlen > 0) err = 0;
        
    return err;
}
unsigned short Client::in_cksum(unsigned short *addr, int len)
{
    int sum         = 0;
    uint16_t answer = 0;
    uint16_t *w     = addr;
    int nleft       = len;
    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }
    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
        *(u_char *)(&answer) = *(u_char *) w;
        sum += answer;
    }
    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
    sum += (sum >> 16);             /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);
}
// end of file
