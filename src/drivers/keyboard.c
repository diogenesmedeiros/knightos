#include <drivers/keyboard.h>

static bool shift_pressed = false;
static bool altgr_pressed = false;
static char dead_key = 0;

const char normal_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','´','[','\n',0,'a','s',
    'd','f','g','h','j','k','l','ç','~',']',0,'\\','z','x','c','v',
    'b','n','m',',','.',';',0,'*',0,' ',0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,'-',0,0,0,'+','<',0,0,0
};

const char shift_map[128] = {
    0, 27, '!','@','#','$','%','¨','&','*','(',')','_','+','\b','\t',
    'Q','W','E','R','T','Y','U','I','O','P','`','{','\n',0,'A','S',
    'D','F','G','H','J','K','L','Ç','^','}',0,'|','Z','X','C','V',
    'B','N','M','<','>',';',0,'*',0,' ',0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,'-',0,0,0,'+','>',0,0,0
};

const char altgr_map[128] = {
    [0x10] = '@', [0x11] = 'ł', [0x1F] = 'ß', [0x02] = '¹', [0x03] = '²', [0x04] = '³'
};

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

char scancode_to_char(uint8_t scancode) {
    char c = 0;

    if (altgr_pressed) c = altgr_map[scancode];
    else if (shift_pressed) c = shift_map[scancode];
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

void keyboard_poll() {
    static uint8_t last_scancode = 0;
    uint8_t scancode = inb(0x60);

    if (scancode & 0x80) {
        scancode &= 0x7F;
        if ((scancode & 0x7F) == last_scancode) last_scancode = 0;
        else if (scancode == 0x2A || scancode == 0x36) shift_pressed = false;
        else if (scancode == 0x38) altgr_pressed = false;

        return;
    }

    if (scancode == 0x2A || scancode == 0x36) { shift_pressed = true; return; }
    if (scancode == 0x38) { altgr_pressed = true; return; }

    if (scancode == last_scancode) return;
    last_scancode = scancode;

    char c = scancode_to_char(scancode);
    if (!c || c == '?') return;

    shell_handle_char(c);
}