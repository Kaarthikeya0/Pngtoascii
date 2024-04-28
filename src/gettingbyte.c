#include <stdint.h>
#include <stdio.h>

uint32_t read_bytes_be32(FILE *file) {
    uint8_t buffer = 0;
    uint32_t doubleword = 0;
    for (int i = 0; i < 4; i++) {
        fread(&buffer, sizeof(uint8_t), 1, file);
        doubleword += buffer;
        if (i < 3)
            doubleword = doubleword << 8;
    }
    return doubleword;
}

uint8_t read_byte(FILE *file) {
    uint8_t byte = 0;
    fread(&byte, sizeof(uint8_t), 1, file);
    return byte;
}
