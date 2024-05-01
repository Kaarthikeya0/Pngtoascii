#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

struct imageHeader {
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t colour_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
};

typedef struct triple {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    
} triple;

typedef struct plteArray {
    uint32_t nmemb;
    triple **pltearray;
} plteArray;

#endif // !STRUCTS_H
