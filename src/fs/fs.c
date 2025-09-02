#include <fs/fs.h>
#include <kernel/terminal.h>
#include <drivers/ata.h>
#include <lib/string.h>

/**
 * Implementação de um sistema de arquivos extremamente simples,
 * baseado em setores de 512 bytes em disco.
 * 
 * Cada diretório é armazenado em um setor.
 * Estrutura do setor de diretório (512 bytes):
 *   [0]      = número de entradas (arquivos + subdiretórios)
 *   [1..32]  = primeira entrada (até 32 bytes cada)
 *   [33..64] = segunda entrada
 *   ...
 * Campos relevantes de uma entrada (32 bytes):
 *   [0..14]  = nome (15 bytes máx, sem '\0')
 *   [15]     = reservado
 *   [16]     = tipo (0x01 = diretório, 0x02 = arquivo)
 *   [17]     = setor onde o conteúdo está
 *   [18..31] = reservado
 */

 // Caminho atual no sistema de arquivos, manipulado manualmente como string
static char current_directory[256] = "/";

// Setor correspondente ao diretório atual
static uint8_t current_sector = 1;

/**
 * Retorna o caminho do diretório atual como string.
 */
const char* fs_get_current_directory() {
    return current_directory;
}

/**
 * Retorna o número do setor associado ao diretório atual.
 */
uint8_t fs_get_current_sector() {
    return current_sector;
}

/**
 * Altera o diretório atual do sistema de arquivos.
 * 
 * @param path Nome do diretório de destino
 *      ".." -> sobe um nível (diretório pai)
 *      outro -> entra no subdiretório indicado
 */
void fs_set_current_directory(const char* path) {
    // Caso especial: subir um nível ("..")
    if (strcmp(path, "..") == 0) {
        int len = strlen(current_directory);
        // não sobe acima da raiz
        if (len > 1) { 
            // Procura última '/' e corta o caminho
            for (int i = len - 2; i >= 0; i--) {
                if (current_directory[i] == '/') {
                    current_directory[i + 1] = '\0';
                    break;
                }
            }

            current_sector = 1; // setor raiz
        }
        return;
    }

    // Lê o setor do diretório atual
    uint8_t sector[512];
    ata_read_sector(current_sector, sector);

    uint8_t num_dirs = sector[0]; // número de entradas no setor
    for (int i = 0; i < num_dirs; i++) {
        char* name = (char*)&sector[1 + i * 32]; // nome da entrada
        uint8_t sub_sector = sector[1 + i * 32 + 17]; // setor correspondente

        // Compara nomes
        if (strncmp(name, path, 15) == 0) {
            // Atualiza o caminho do diretório atual
            if (strcmp(current_directory, "/") != 0) {
                strcat(current_directory, path);
                strcat(current_directory, "/");
            } else {
                strcpy(current_directory, "/");
                strcat(current_directory, path);
                strcat(current_directory, "/");
            }

            // Atualiza setor atual
            current_sector = sub_sector;
            return;
        }
    }

    // Diretório não encontrado
    terminal_print("Directory not found.\n");
}

/**
 * Procura um arquivo dentro do diretório atual e retorna
 * o setor onde ele está armazenado.
 * 
 * @param name Nome do arquivo (máx 15 chars)
 * @return Número do setor do arquivo, ou -1 se não encontrado
 */
int fs_find(const char* name) {
    uint8_t sector[512];
    uint8_t dir_sector = fs_get_current_sector();
    ata_read_sector(dir_sector, sector); // lê setor atual

    int num_entries = sector[0]; // número de entradas neste diretório
    for (int i = 0; i < num_entries; i++) {
        uint8_t* entry = &sector[1 + i * 32];

        // Verifica se a entrada é do tipo "arquivo" (0x02)
        if (entry[16] == 0x02) {
            char entry_name[16] = {0};
            memcpy(entry_name, entry, 15);

            // Compara nome do arquivo
            if (strncmp(entry_name, name, strlen(name)) == 0) {
                return entry[17]; // retorna setor do arquivo
            }
        }
    }
    return -1; // arquivo não encontrado
}