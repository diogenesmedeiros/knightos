#include <drivers/rtl8139.h>
#include <kernel/terminal.h>
#include <lib/string.h>
#include <stdint.h>
#include <drivers/dhcp.h>
#include <net/net_config.h>

#define DHCP_CLIENT_PORT 68
#define DHCP_SERVER_PORT 67
#define DHCP_MAGIC_COOKIE 0x63825363
#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_OPTION_MSG_TYPE 53
#define DHCP_OPTION_END 255
#define DHCP_OPTION_SUBNET 1
#define DHCP_OPTION_GATEWAY 3

typedef struct {
    uint8_t op, htype, hlen, hops;
    uint32_t xid;
    uint16_t secs, flags;
    uint32_t ciaddr, yiaddr, siaddr, giaddr;
    uint8_t chaddr[16];
    uint8_t sname[64];
    uint8_t file[128];
    uint32_t magic_cookie;
    uint8_t options[312];
} __attribute__((packed)) dhcp_packet_t;

void send_dhcp_discover() {
    dhcp_packet_t pkt;
    memset(&pkt, 0, sizeof(pkt));
    pkt.op = 1; pkt.htype = 1; pkt.hlen = 6;
    pkt.xid = 0x12345678;
    memcpy(pkt.chaddr, netdev.mac, 6);
    pkt.magic_cookie = htonl(DHCP_MAGIC_COOKIE);

    uint8_t *opt = pkt.options;
    *opt++ = DHCP_OPTION_MSG_TYPE;
    *opt++ = 1;
    *opt++ = DHCP_DISCOVER;
    *opt++ = DHCP_OPTION_END;

    uint8_t broadcast_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    rtl8139_send((uint8_t*)&pkt, sizeof(pkt));
}

int handle_dhcp_offer() {
    uint8_t buffer[1500];
    size_t len = rtl8139_receive(buffer, sizeof(buffer));
    if(len < sizeof(dhcp_packet_t)) return 0;

    dhcp_packet_t *pkt = (dhcp_packet_t*)buffer;
    if(pkt->op != 2) return 0;
    if(ntohl(pkt->magic_cookie) != DHCP_MAGIC_COOKIE) return 0;

    my_ip = pkt->yiaddr;

    int i = 0;
    while(i < sizeof(pkt->options)) {
        uint8_t opt = pkt->options[i++];
        if(opt == DHCP_OPTION_END) break;
        uint8_t len = pkt->options[i++];
        if(opt == DHCP_OPTION_SUBNET) netmask = *((uint32_t*)(pkt->options + i));
        else if(opt == DHCP_OPTION_GATEWAY) gateway = *((uint32_t*)(pkt->options + i));
        i += len;
    }

    return 1;
}

void dhcp_init() {
    send_dhcp_discover();
    int attempts = 100000;
    while(attempts-- > 0) {
        if(handle_dhcp_offer()) break;
    }
}