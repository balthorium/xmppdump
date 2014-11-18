#include <arpa/inet.h>

#define ETHER_PKT_TYPE_RECV_UCAST    0x0000 
#define ETHER_PKT_TYPE_RECV_BCAST    0x0001 
#define ETHER_PKT_TYPE_RECV MCAST    0x0002 
#define ETHER_PKT_TYPE_NOT_OURS      0x0003 
#define ETHER_PKT_TYPE_SEND          0x0004 
#define ETHER_DEV_TYPE               0x0001
#define ETHER_PROTOCOL_IP4           0x0800
#define ETHER_PROTOCOL_ARP           0x0806
#define ETHER_PROTOCOL_IP6           0x86DD

#define IP_PROTOCOL_ICMP             1
#define IP_PROTOCOL_IGMP             2
#define IP_PROTOCOL_TCP              6
#define UP_PROTOCOL_UDP              17

#define TCP_FLAG_URG                 0x20
#define TCP_FLAG_ACK                 0x10
#define TCP_FLAG_PSH                 0x08
#define TCP_FLAG_RST                 0x04
#define TCP_FLAG_SYN                 0x02
#define TCP_FLAG_FIN                 0x01

struct dlt_linux_sll_hdr 
{
    uint16_t pkt_type;
    uint16_t dev_type;
    uint16_t addr_len;
    uint8_t src_mac[6];
    uint8_t pad[2];
    uint16_t protocol;
};    

struct ip_hdr 
{
    uint8_t hdr_len :4; 
    uint8_t version :4;
    uint8_t tos;
    uint16_t pkt_len;
    uint16_t pkt_id;
    uint8_t flags   :3;
    uint16_t offset :13;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t hdr_checksum; 
    uint8_t src[4];
    uint8_t dst[4];
};

struct tcp_hdr
{
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seqnum;
    uint32_t acknum;
    uint8_t  rsvd1   :4;
    uint8_t  hdr_len :4;
    uint8_t  flags   :6;
    uint8_t  rsvd2   :2;
    uint16_t windsz;
    uint16_t tcp_checksum;
    uint16_t urg_pointer;
};
 
