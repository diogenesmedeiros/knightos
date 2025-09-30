#include <drivers/rtl8139.h>
#include <kernel/terminal.h>
#include <lib/string.h>
#include <stdint.h>

// Config
#define RTL8139_BASE    0xC000
#define RX_BUFFER_SIZE  8192
#define TX_BUFFER_SIZE  2048

rtl8139_device_t netdev;

// I/O
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0,%1"::"a"(val),"Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1,%0":"=a"(ret):"Nd"(port));
    
    return ret; 
}

static inline void outl(uint16_t port, uint32_t val) {
    asm volatile ("outl %0,%1"::"a"(val),"Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ("inl %1,%0":"=a"(ret):"Nd"(port));
    
    return ret;
}

// Stub de alocação DMA
static void *dma_alloc_stub(size_t size) {
    void *p = kmalloc(size + 16);  // kmalloc deve existir em memory.c
    if (!p) return NULL;
    memset(p, 0, size + 16);
    return p;
}

void rtl8139_init(void) {
    outb(RTL8139_BASE + 0x37, 0x10);  // reset
    while(inb(RTL8139_BASE + 0x37) & 0x10) {}

    netdev.rx_buffer = dma_alloc_stub(RX_BUFFER_SIZE + 16);
    if(!netdev.rx_buffer) { terminal_print("falha ao alocar RX buffer\n"); return; }

    for(int i=0;i<4;i++){
        netdev.tx_buffers[i] = dma_alloc_stub(TX_BUFFER_SIZE);
        if(!netdev.tx_buffers[i]) { terminal_print("falha ao alocar TX buffer\n"); return; }
    }

    outl(RTL8139_BASE + 0x30, (uint32_t)netdev.rx_buffer);
    for(int i=0;i<4;i++) outl(RTL8139_BASE + 0x20 + i*4, (uint32_t)netdev.tx_buffers[i]);

    outb(RTL8139_BASE + 0x37, 0x0C);  // habilita RX/TX
    netdev.tx_index = 0;

    for(int i = 0; i < 6; i++)
        netdev.mac[i] = inb(RTL8139_BASE + i);

    // MAC fictício
    // netdev.mac[0]=0x52; netdev.mac[1]=0x54; netdev.mac[2]=0x00;
    // netdev.mac[3]=0x12; netdev.mac[4]=0x34; netdev.mac[5]=0x56;
}

void rtl8139_send(uint8_t *data, size_t len){
    if(!data || len==0 || len>TX_BUFFER_SIZE) return;
    int idx = netdev.tx_index;
    memcpy(netdev.tx_buffers[idx], data, len);
    outl(RTL8139_BASE + 0x10 + idx*4, (uint32_t)len);
    netdev.tx_index = (idx + 1) & 0x3;
}

size_t rtl8139_receive(uint8_t *out_buf, size_t out_cap){
    static uint32_t rx_ptr = 0;
    if(!out_buf || out_cap==0 || !netdev.rx_buffer) return 0;

    uint32_t header;
    if(rx_ptr + 4 <= RX_BUFFER_SIZE){
        header = *((uint32_t*)(netdev.rx_buffer + rx_ptr));
    }else{
        uint8_t tmp[4];
        for(int i=0;i<4;i++) tmp[i] = netdev.rx_buffer[(rx_ptr+i)%RX_BUFFER_SIZE];
        header = *((uint32_t*)tmp);
    }

    uint16_t status = header & 0xFFFF;
    uint16_t pkt_len = (header >> 16) & 0xFFFF;
    if((status & 0x01)==0 || pkt_len==0 || pkt_len>1500){ rx_ptr=(rx_ptr+4)%RX_BUFFER_SIZE; return 0; }

    uint32_t payload_off = (rx_ptr + 4) % RX_BUFFER_SIZE;
    size_t to_copy = (pkt_len>out_cap)? out_cap : pkt_len;
    uint32_t first_chunk = RX_BUFFER_SIZE - payload_off;

    if(first_chunk >= to_copy) memcpy(out_buf, netdev.rx_buffer + payload_off, to_copy);
    else {
        memcpy(out_buf, netdev.rx_buffer + payload_off, first_chunk);
        memcpy(out_buf + first_chunk, netdev.rx_buffer, to_copy - first_chunk);
    }

    rx_ptr = (rx_ptr + ((4+pkt_len+3)&~3)) % RX_BUFFER_SIZE;
    outl(RTL8139_BASE + 0x38, rx_ptr);
    return to_copy;
}
