#include <stdint.h>
#include <stdio.h>

uint32_t read_bytes_be32(FILE *file) {
    uint8_t buffer = 0;
    uint32_t dword = 0;
    for (int i = 0; i < 4; i++) {
        fread(&buffer, sizeof(uint8_t), 1, file);
        dword += buffer;
        if (i < 3)
            dword = dword << 8;
    }
    return dword;
}

uint16_t read_bytes_be16(FILE *file) {
    uint8_t buffer = 0; 
    uint16_t word = 0;
    for (int i = 0; i < 2; i++) {
        fread(&buffer, sizeof(uint8_t), 1, file);
        word += buffer;
        if (i == 0)
            word = word << 8;
    }
    return word;
}

uint8_t read_byte(FILE *file) {
    uint8_t byte = 0;
    fread(&byte, sizeof(uint8_t), 1, file);
    return byte;
}
