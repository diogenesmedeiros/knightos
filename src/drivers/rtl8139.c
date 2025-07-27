#include "rtl8139.h"
#include <drivers/io.h>
#include <kernel/interrupts.h>
#include <kernel/terminal.h>
#include <string.h>

#define RTL8139_BASE_PORT 0xC000
#define RX_BUF_SIZE 8192
#define TX_BUF_SIZE 2048

#define RTL_CR      0x37
#define RTL_ISR     0x3E
#define RTL_IMR     0x3C
#define RTL_RBSTART 0x30
#define RTL_CAPR    0x38
#define RTL_TSD0    0x10
#define RTL_TSAD0   0x20

static uint8_t rx_buffer[RX_BUF_SIZE] __attribute__((aligned(4)));
static volatile uint32_t rx_offset = 0;

static uint8_t tx_buffers[4][TX_BUF_SIZE] __attribute__((aligned(4)));
static volatile int tx_current = 0;

static void rtl8139_reset() {
    outb(RTL8139_BASE_PORT + RTL_CR, 0x10);
    while (inb(RTL8139_BASE_PORT + RTL_CR) & 0x10);
}

void rtl8139_init() {
    rtl8139_reset();

    outl(RTL8139_BASE_PORT + RTL_RBSTART, (uint32_t)rx_buffer);
    outw(RTL8139_BASE_PORT + RTL_IMR, 0x0005);
    outb(RTL8139_BASE_PORT + RTL_CR, 0x0C);

    rx_offset = 0;
    tx_current = 0;

    terminal_print("RTL8139 initialized\n");
}

void rtl8139_handle_irq() {
    uint16_t isr = inw(RTL8139_BASE_PORT + RTL_ISR);
    outw(RTL8139_BASE_PORT + RTL_ISR, isr);

    if (isr & 0x01) {
        while (1) {
            if (rx_offset == *((uint16_t*)(rx_buffer + 0))) break;

            uint16_t pkt_len = *((uint16_t*)(rx_buffer + rx_offset + 2));
            uint8_t* pkt_data = rx_buffer + rx_offset + 4;

            rx_offset += ((pkt_len + 4 + 3) & ~3);
            if (rx_offset >= RX_BUF_SIZE) rx_offset = 0;
        }
    }

    if (isr & 0x04) {
    }
}

int rtl8139_send_packet(const void* data, uint16_t len) {
    int idx = tx_current;
    tx_current = (tx_current + 1) % 4;

    memcpy(tx_buffers[idx], data, len);
    outl(RTL8139_BASE_PORT + RTL_TSAD0 + idx * 4, (uint32_t)tx_buffers[idx]);
    outl(RTL8139_BASE_PORT + RTL_TSD0 + idx * 4, len);

    return 0;
}

void rtl8139_install() {
    register_interrupt_handler(IRQ_ETHERNET, rtl8139_handle_irq);
    rtl8139_init();
}