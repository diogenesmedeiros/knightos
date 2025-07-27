#include <stdint.h>
#include <kernel/memory.h>
#include <lib/string.h>

#define PAGE_SIZE 4096

static uint64_t g_total_ram = 0;
static uint32_t g_max_pages = 0;
static uint8_t* g_page_bitmap = 0;

// Marca a página 'page' como usada no bitmap global de páginas (g_page_bitmap)
static inline void set_page(uint32_t page) {
    g_page_bitmap[page / 8] |= (1 << (page % 8));
}

// Marca a página 'page' como livre no bitmap global de páginas (g_page_bitmap)
static inline void clear_page(uint32_t page) {
    g_page_bitmap[page / 8] &= ~(1 << (page % 8));
}

/**
 * Verifica se a página 'page' está marcada como usada no bitmap global (g_page_bitmap)
 * Retorna diferente de zero se usada, 0 se livre
 */
static inline int test_page(uint32_t page) {
    return g_page_bitmap[page / 8] & (1 << (page % 8));
}

/**
 * Detecta memória total disponível usando a estrutura multiboot info
 * Verifica se o mapa de memória está presente (flag bit 6)
 * Soma os blocos de memória tipo 1 (RAM utilizável) e retorna o total em bytes
 */
uint64_t detect_memory(uint32_t magic, uint32_t addr) {
    if (magic != 0x2BADB002)
        return 0;

    typedef struct {
        uint32_t size;
        uint64_t addr;
        uint64_t len;
        uint32_t type;
    } __attribute__((packed)) memory_map_t;

    typedef struct {
        uint32_t flags;
        uint32_t mem_lower;
        uint32_t mem_upper;
        uint32_t boot_device;
        uint32_t cmdline;
        uint32_t mods_count;
        uint32_t mods_addr;
        uint32_t syms[4];
        uint32_t mmap_length;
        uint32_t mmap_addr;
    } __attribute__((packed)) multiboot_info_t;

    multiboot_info_t* mbi = (multiboot_info_t*) addr;

    if (!(mbi->flags & (1 << 6)))
        return 0;

    uint64_t total_memory = 0;
    memory_map_t* mmap = (memory_map_t*) mbi->mmap_addr;

    uint32_t end = mbi->mmap_addr + mbi->mmap_length;

    while ((uint32_t)mmap < end) {
        if (mmap->type == 1) {
            total_memory += mmap->len;
        }
        mmap = (memory_map_t*)((uint32_t)mmap + mmap->size + 4);
    }

    g_total_ram = total_memory;
    return total_memory;
}

/**
 * Inicializa gerenciador de memória por paginação
 * Calcula número total de páginas, aloca bitmap após kernel alinhado a PAGE_SIZE
 * Marca páginas ocupadas pelo kernel e pelo bitmap como usadas
 * Zera o bitmap antes de marcar páginas usadas
 */
void memory_init(uint64_t total_ram, uint32_t kernel_start, uint32_t kernel_end_addr) {
    g_max_pages = total_ram / PAGE_SIZE;

    uint32_t bitmap_size = (g_max_pages + 7) / 8;  
    g_page_bitmap = (uint8_t*)((kernel_end_addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));

    memset(g_page_bitmap, 0, bitmap_size);

    uint32_t start_page = kernel_start / PAGE_SIZE;
    uint32_t end_page = (kernel_end_addr + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t i = start_page; i < end_page; i++) {
        set_page(i);
    }

    uint32_t bitmap_start = (uint32_t)g_page_bitmap;
    uint32_t bitmap_end = bitmap_start + bitmap_size;
    uint32_t b_start_page = bitmap_start / PAGE_SIZE;
    uint32_t b_end_page = (bitmap_end + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t i = b_start_page; i < b_end_page; i++) {
        set_page(i);
    }
}

/**
 * Aloca uma página livre no gerenciador de memória
 * Busca a primeira página livre no bitmap, marca como usada, zera seu conteúdo e retorna endereço físico]
 * Retorna 0 se não houver páginas disponíveis
 */
uint32_t alloc_page(void) {
    for (uint32_t i = 0; i < g_max_pages; i++) {
        if (!test_page(i)) {
            set_page(i);
            uint8_t* mem = (uint8_t*)(i * PAGE_SIZE);
            memset(mem, 0, PAGE_SIZE); 
            return i * PAGE_SIZE;
        }
    }
    return 0;
}

void free_page(uint32_t addr) {
    uint32_t page = addr / PAGE_SIZE;
    clear_page(page);
}