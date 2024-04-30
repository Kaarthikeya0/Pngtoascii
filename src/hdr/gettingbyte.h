#ifndef GETTINGBYTE_H
#define GETTINGBYTE_H
#include <stdint.h>
#include <stdio.h>

uint32_t read_bytes_be32(FILE *file);
uint8_t read_byte(FILE *file);
#endif // !GETTINGBYTE_H
