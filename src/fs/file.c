#include <fs/file.h>
#include <fs/fs.h>
#include <kernel/terminal.h>
#include <drivers/ata.h>
#include <lib/string.h>
#include <lib/stdlib.h>
#include <stdint.h>

/**
 * Lê um arquivo simples do disco ATA usando o nome extraído do caminho
 * Busca o setor com fs_find, lê 512 bytes com ata_read_sector, copia 
 * até o byte nulo para um buffer alocado e retorna
 */
char* read_file(const char* path) {
    // Extrai apenas o nome do arquivo do caminho.
    const char* name = strrchr(path, '/');
    if (!name) name = path; // Se não tem '/', o nome é o próprio path
    else name++; // Pula a '/' para pegar apenas o nome

    /**
     * Procura o setor LBA onde o arquivo está armazenado.
     * A função fs_find deve retornar o número do setor do arquivo.
     */
    int sector = fs_find(name);

    // Caso não encontre o arquivo
    if (sector < 0) {
        terminal_print("File not found: ");
        terminal_print(name);
        terminal_print("\n");
        return NULL;
    }

    // Buffer temporário para armazenar os 512 bytes do setor lido do disco
    uint8_t data_sector[512];
    ata_read_sector(sector, data_sector); // Lê 1 setor do disco para o buffer

    // Aloca memória dinâmica para copiar o conteúdo do arquivo
    char* content = malloc(512);

    // Se falhar na alocação
    if (!content) {
        terminal_print("Memory error.\n");
        return NULL;
    }

    /**
     * Copia os bytes do setor para a memória recém-alocada.
     * Para ao encontrar '\0' (fim de string) ou no limite de 511 bytes
     */
    int i;
    for (i = 0; i < 511 && data_sector[i]; i++) {
        content[i] = data_sector[i];
    }
    content[i] = '\0'; // Garante que a string termine corretamente

    // Retorna a string com o conteúdo do arquivo
    return content;
}