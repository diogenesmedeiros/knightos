#include <drivers/rtl8139.h>
#include <net/net.h>
#include <kernel/terminal.h>
#include <lib/string.h>
#include <stdint.h>

extern rtl8139_device_t netdev;

uint16_t checksum(void *buf, int len){
    uint32_t sum=0;
    uint16_t *ptr=buf;
    while(len>1){ sum += *ptr++; len-=2; }
    if(len) sum += *((uint8_t*)ptr);
    while(sum>>16) sum=(sum & 0xFFFF)+(sum>>16);
    return ~sum;
}

void send_icmp_ping(uint32_t src_ip, uint32_t dst_ip, uint8_t *dst_mac){
    uint8_t packet[1500];
    memset(packet, 0, sizeof(packet));

    eth_header_t *eth = (eth_header_t*)packet;
    for(int i=0;i<6;i++){ eth->dest[i]=dst_mac[i]; eth->src[i]=netdev.mac[i]; }
    eth->type=htons(ETH_P_IP);

    ip_header_t *ip=(ip_header_t*)(packet + sizeof(eth_header_t));
    ip->ver_ihl=0x45; ip->tos=0;
    ip->tot_len=htons(sizeof(ip_header_t)+sizeof(icmp_header_t));
    ip->id=0; ip->frag_off=0; ip->ttl=64; ip->proto=1;
    ip->src_addr=htonl(src_ip); ip->dest_addr=htonl(dst_ip);
    ip->checksum=0; ip->checksum=checksum(ip,sizeof(ip_header_t));

    icmp_header_t *icmp=(icmp_header_t*)((uint8_t*)ip + sizeof(ip_header_t));
    icmp->type=ICMP_ECHO; icmp->code=0; icmp->id=1; icmp->seq=1; icmp->checksum=0;
    icmp->checksum=checksum(icmp,sizeof(icmp_header_t));

    rtl8139_send(packet, sizeof(eth_header_t)+sizeof(ip_header_t)+sizeof(icmp_header_t));
}

void handle_icmp_reply(){
    uint8_t buffer[1500];
    size_t len = rtl8139_receive(buffer, sizeof(buffer));
    if(len == 0) return;
    if(len < sizeof(eth_header_t)+sizeof(ip_header_t)+sizeof(icmp_header_t)) return;

    eth_header_t *eth = (eth_header_t*)buffer;
    if(ntohs(eth->type)!=ETH_P_IP) return;

    ip_header_t *ip = (ip_header_t*)(buffer + sizeof(eth_header_t));
    if(ip->proto!=1) return;

    icmp_header_t *icmp = (icmp_header_t*)((uint8_t*)ip + (ip->ver_ihl&0xF)*4);
    if(icmp->type == 0) terminal_print("Ping reply recebido!\n");
}
