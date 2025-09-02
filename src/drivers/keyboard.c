#include <stdbool.h>
#include <stdint.h>
#include <drivers/io.h>
#include <drivers/keyboard.h>

static bool shift_pressed = false;
static bool altgr_pressed = false;
static bool ctrl_pressed = false;
static char dead_key = 0;

static int special_key = 0;

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int keyboard_buffer_index = 0;

/**
 * Mapeamento de scancodes para caracteres ASCII em layout ABNT2 (sem Shift)
 * Usado para converter códigos de tecla do teclado PS/2 em caracteres legíveis
 */
const char normal_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','´','[','\n',0,'a','s',
    'd','f','g','h','j','k','l','ç','~',']',0,'\\','z','x','c','v',
    'b','n','m',',','.',';',0,'*',0,' ',0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,'-',0,0,0,'+','<',0,0,0
};

/**
 * Mapeamento de scancodes para caracteres ASCII em layout ABNT2 com Shift ativado
 * Converte códigos de tecla PS/2 em caracteres maiúsculos e símbolos especiais
 */
const char shift_map[128] = {
    0, 27, '!','@','#','$','%','¨','&','*','(',')','_','+','\b','\t',
    'Q','W','E','R','T','Y','U','I','O','P','`','{','\n',0,'A','S',
    'D','F','G','H','J','K','L','Ç','^','}',0,'|','Z','X','C','V',
    'B','N','M',';',':','/',0,'*',0,' ',0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,'_',0,0,0,'+','>',0,0,0
};

/**
 * Mapeamento parcial de scancodes para caracteres ASCII especiais com AltGr (layout ABNT2)
 * Traduz teclas específicas para símbolos acentuados e monetários ao usar AltGr
 */
const char altgr_map[128] = {
    [0x10] = '@',
    [0x11] = 'ł',
    [0x1F] = 'ß',
    [0x02] = '¹',
    [0x03] = '²',
    [0x04] = '³',
    [0x0C] = '¢',
    [0x18] = '€',
    [0x19] = '£',
    [0x2B] = '\\',
    [0x27] = '~',
    [0x28] = '"',
    [0x34] = '>',
    [0x33] = '<'
};

/**
 * Combina tecla morta (dead key) com caractere base 
 * para retornar caractere acentuado correspondente.
 * Suporta acentos agudo (´), grave (`), til (~) e circunflexo (^)
 * Retorna 0 se a combinação não for válida
 */
char compose_dead_key(char dead, char base) {
    if (dead == '´') {
        switch (base) {
            case 'a': return 'á'; case 'e': return 'é'; case 'i': return 'í';
            case 'o': return 'ó'; case 'u': return 'ú';
            case 'A': return 'Á'; case 'E': return 'É'; case 'I': return 'Í';
            case 'O': return 'Ó'; case 'U': return 'Ú';
        }
    }
    if (dead == '`') {
        switch (base) {
            case 'a': return 'à'; case 'e': return 'è'; case 'i': return 'ì';
            case 'o': return 'ò'; case 'u': return 'ù';
        }
    }
    if (dead == '~') {
        switch (base) {
            case 'a': return 'ã'; case 'o': return 'õ'; case 'n': return 'ñ';
            case 'A': return 'Ã'; case 'O': return 'Õ'; case 'N': return 'Ñ';
        }
    }
    if (dead == '^') {
        switch (base) {
            case 'a': return 'â'; case 'e': return 'ê'; case 'i': return 'î';
            case 'o': return 'ô'; case 'u': return 'û';
        }
    }
    return 0;
} 

/**
 * Converte scancode em caractere ASCII considerando AltGr, Shift e tecla morta (dead key)
 * Prioriza AltGr, depois Shift, depois mapa normal
 * Armazena dead key para compor caracteres acentuados na próxima tecla
 * Retorna '?' se scancode inválido, 0 se aguarda composição, ou caractere final
 */
char scancode_to_char(uint8_t scancode) {
    char c = 0;

    if (altgr_pressed && altgr_map[scancode]) c = altgr_map[scancode];
    else if (shift_pressed && shift_map[scancode]) c = shift_map[scancode];
    else c = normal_map[scancode];

    if (!c) return '?';

    if (c == '´' || c == '`' || c == '~' || c == '^') {
        dead_key = c;
        return 0;
    }

    if (dead_key) {
        char composed = compose_dead_key(dead_key, c);
        dead_key = 0;
        if (composed) return composed;
    }

    return c;
}

/**
 * Lê e processa scancodes do teclado PS/2
 * Atualiza estados de Shift, AltGr e Ctrl com base em press/release
 * Detecta teclas especiais (seta cima/baixo)
 * Ignora repetições imediatas do mesmo scancode
 * Converte scancode em caractere, aplica controles Ctrl+C/X/V e envia para buffer de teclado
 */
void keyboard_poll() {
    static uint8_t last_scancode = 0;
    uint8_t scancode = inb(0x60);

    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == last_scancode) last_scancode = 0;

        if (scancode == 0x2A || scancode == 0x36) shift_pressed = false;
        else if (scancode == 0x38) altgr_pressed = false;
        else if (scancode == 0x1D) ctrl_pressed = false;

        return;
    }

    if (scancode == 0x2A || scancode == 0x36) { shift_pressed = true; return; }
    if (scancode == 0x38) { altgr_pressed = true; return; }
    if (scancode == 0x1D) { ctrl_pressed = true; return; }

    if (scancode == 0x48) {
        special_key = KEY_UP;
        return;
    }

    if (scancode == 0x50) {
        special_key = KEY_DOWN;
        return;
    }

    if (scancode == last_scancode) return;
    last_scancode = scancode;

    char c = scancode_to_char(scancode);
    if (!c || c == '?') return;

    if (ctrl_pressed) {
        if (c == 'x' || c == 'X') c = 24;
        else if (c == 'c' || c == 'C') c = 3;
        else if (c == 'v' || c == 'V') c = 22;
    }

    keyboard_put_char(c);
}

// Retorna a última tecla especial detectada e reseta o estado para 0
int keyboard_get_special_key() {
    int key = special_key;
    special_key = 0;
    return key;
}

// Insere caractere no buffer do teclado se houver espaço disponível
void keyboard_put_char(char c) {
    if (keyboard_buffer_index < KEYBOARD_BUFFER_SIZE) {
        keyboard_buffer[keyboard_buffer_index++] = c;
    }
}

/**
 * Retorna o próximo caractere do buffer do teclado, bloqueando até haver dados
 * Desloca o buffer para frente após leitura
 */
char keyboard_get_char() {
    while (keyboard_buffer_index == 0);
    char c = keyboard_buffer[0];
    for (int i = 1; i < keyboard_buffer_index; i++) {
        keyboard_buffer[i - 1] = keyboard_buffer[i];
    }
    keyboard_buffer_index--;
    return c;
}

/**
 * Lê caractere do teclado de forma bloqueante, ignorando códigos de tecla solta (release)
 * Converte scancode em caractere e retorna assim que válido
 */
char keyboard_read_char_blocking() {
    while (1) {
        uint8_t scancode = inb(0x60);

        if (scancode & 0x80) {
            continue;
        }

        char c = scancode_to_char(scancode);
        if (c) return c;
    }
}

// Retorna se há caracteres disponíveis no buffer do teclado (não bloqueante)
int keyboard_has_char() {
    return keyboard_buffer_index > 0;
}

// Esvazia o buffer do teclado descartando todos os caracteres disponíveis
void keyboard_clear_buffer() {
    while (keyboard_has_char()) {
        keyboard_get_char();
    }
}