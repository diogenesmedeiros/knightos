#include <drivers/io.h>

#define ATA_PRIMARY_IO  0x1F0
#define ATA_PRIMARY_CTRL 0x3F6

/**
 * Lê 1 setor (512 bytes) de um disco ATA usando LBA e armazena no buffer
 * Envia comandos à porta ATA primária (0x1F0–0x1F7) para configurar o LBA, iniciar a leitura (0x20),
 * espera até o disco estar pronto (bit DRQ ativado), e então lê os 256 words (16 bits cada) da porta de dados.
 */
void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    /**
     * 0x1F6: é o registrador de "drive/head".
     * 0xE0: modo LBA + drive 0 (master).
     * Pega os 4 bits altos do endereço LBA.
     */
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));

    // Vai ler 1 setor (512 bytes).
    outb(0x1F2, 1);

    outb(0x1F3, (uint8_t)(lba)); // bits 0..7
    outb(0x1F4, (uint8_t)(lba >> 8)); // bits 8..15
    outb(0x1F5, (uint8_t)(lba >> 16)); // bits 16..23
    outb(0x1F7, 0x20); // 0x20: comando ATA "READ SECTOR(S)".

    /**
     * Espera disco sinalizar pronto para transferência de dados
     * 0x08: bit DRQ (Data Request).
     */
    while (!(inb(0x1F7) & 0x08));

    /**
     * Lê os 512 bytes do setor
     * Cada inw(0x1F0) lê 16 bits (2 bytes) da porta de dados.
     * 256 leituras × 2 bytes = 512 bytes = 1 setor.
     * O resultado vai para o buffer.
     */
    for (int i = 0; i < 256; i++) {
        ((uint16_t*)buffer)[i] = inw(0x1F0);
    }
}

/**
 * Escreve 1 setor (512 bytes) no disco ATA usando LBA a partir do buffer
 * Configura o endereço LBA nas portas ATA (0x1F3–0x1F6), espera o disco estar pronto,
 * envia o comando de escrita (0x30), espera o bit DRQ, e escreve 256 words (16 bits) 
 * na porta de dados (0x1F0).
 */
void ata_write_sector(uint32_t lba, const uint8_t* buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));

    outb(0x1F2, 1);

    outb(0x1F3, (uint8_t)(lba));
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));

    while (inb(0x1F7) & 0x80);

    outb(0x1F7, 0x30);

    while (!(inb(0x1F7) & 0x08));

    for (int i = 0; i < 256; i++) {
        outw(0x1F0, ((const uint16_t*)buffer)[i]);
    }

    while (inb(0x1F7) & 0x80);
}

/**
 * Detecta o tamanho do disco ATA primário em bytes usando o comando IDENTIFY (0xEC)
 * Lê os 512 bytes de resposta, extrai os setores totais dos campos 60 e 61,
 * e retorna o tamanho total em bytes (setores × 512).
 */
uint64_t detect_disk_size() {
    outb(ATA_PRIMARY_IO + 6, 0xA0);
     
    while ((inb(ATA_PRIMARY_IO + 7) & 0x80));

    outb(ATA_PRIMARY_IO + 7, 0xEC);

    if (inb(ATA_PRIMARY_IO + 7) == 0) {
        terminal_print("No disks detected.\n");
        return 0;
    }

    while (!(inb(ATA_PRIMARY_IO + 7) & 0x08));

    uint16_t data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = inw(ATA_PRIMARY_IO);
    }

    uint32_t total_sectors = (data[61] << 16) | data[60];
    uint64_t size_bytes = (uint64_t)total_sectors * 512;

    return size_bytes;
}