#ifndef RTL8139_H
#define RTL8139_H

#include <stdint.h>
#include <net/net.h>

void rtl8139_init();
void rtl8139_send(uint8_t *data, size_t len);
size_t rtl8139_receive(uint8_t *out_buf, size_t out_cap);

typedef struct {
    uint8_t mac[6];
    uint8_t *rx_buffer;
    uint8_t *tx_buffers[4];
    int tx_index;
} rtl8139_device_t;

extern rtl8139_device_t netdev;

#endif
