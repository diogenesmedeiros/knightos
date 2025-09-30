#include <commands/command.h>
#include <kernel/terminal.h>
#include <drivers/rtl8139.h>
#include <net/icmp.h>
#include <lib/string.h>
#include <stdint.h>
#include <drivers/dhcp.h>
#include <net/net_config.h>

int parse_ip(const char* str, uint32_t* out_ip) {
    uint32_t b[4] = {0};
    int part = 0;

    if (!str) return 0;

    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        if (c >= '0' && c <= '9') {
            b[part] = b[part] * 10 + (c - '0');
        } else if (c == '.') {
            part++;
            if (part > 3) return 0;
        } else {
            return 0;
        }
    }
    if (part != 3) return 0;

    *out_ip = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
    return 1;
}

void cmd_ping(const char* arg) {
    if (!arg || *arg == '\0') {
        terminal_print("Usage: ping <ip>\n");
        return;
    }

    uint32_t dst_ip;
    if (!parse_ip(arg, &dst_ip)) {
        terminal_print("Invalid IP format\n");
        return;
    }

    if (my_ip == 0) {
        terminal_print("IP not configured! Running DHCP...\n");
        dhcp_init();
    }

    terminal_print("Pinging ");
    terminal_print(arg);
    terminal_print("...\n");

    // Envia ICMP e espera a resposta
    int attempts = 4;
    for (int i = 0; i < attempts; i++) {
        send_icmp_ping(my_ip, dst_ip, netdev.mac);

        int wait_ticks = 100000;
        int received = 0;
        while (wait_ticks-- > 0) {
            uint8_t buffer[1500];
            size_t len = rtl8139_receive(buffer, sizeof(buffer));
            if (len >= sizeof(eth_header_t) + sizeof(ip_header_t) + sizeof(icmp_header_t)) {
                eth_header_t *eth = (eth_header_t*)buffer;
                if (ntohs(eth->type) != ETH_P_IP) continue;

                ip_header_t *ip = (ip_header_t*)(buffer + sizeof(eth_header_t));
                if (ip->proto != 1) continue;

                icmp_header_t *icmp = (icmp_header_t*)((uint8_t*)ip + (ip->ver_ihl & 0xF) * 4);
                if (icmp->type == 0) { // Echo reply
                    terminal_print("Reply received!\n");
                    received = 1;
                    break;
                }
            }
        }

        if (!received) terminal_print("Request timed out.\n");
    }
}