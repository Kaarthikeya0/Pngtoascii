#include "hdr/structs.h"
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Range {
    uint8_t min;
    uint8_t max;
    char character;
} range;

const range Lookup[] = {
    {0, 19, ' '},
    {19, 21, '`'},
    {21, 31, '.'},
    {31, 35, '\''},
    {35, 39, ':'},
    {39, 47, '_'},
    {47, 55, ','},
    {55, 61, '^'},
    {61, 65, '='},
    {65, 73, ';'},
    {73, 74, '>'},
    {74, 79, '<'},
    {79, 81, '!'},
    {81, 82, 'r'},
    {82, 84, 'c'},
    {84, 86, '*'},
    {86, 92, '/'},
    {92, 93, '?'},
    {93, 95, 's'},
    {95, 98, 'T'},
    {98, 100, 'v'},
    {100, 101, ')'},
    {101, 102, '7'},
    {102, 104, '('},
    {104, 107, 'F'},
    {107, 108, '{'},
    {108, 109, '}'},
    {109, 110, 'I'},
    {110, 112, '3'},
    {112, 113, '1'},
    {113, 114, 'l'},
    {114, 115, '['},
    {115, 116, 'n'},
    {116, 117, 'e'},
    {117, 118, 'o'},
    {118, 119, '5'},
    {119, 120, 'x'},
    {120, 123, 'y'},
    {123, 124, 'a'},
    {124, 126, ']'},
    {126, 127, 'E'},
    {127, 141, 'S'},
    {141, 142, 'w'},
    {142, 143, 'k'},
    {143, 144, 'h'},
    {144, 147, '9'},
    {147, 148, '4'},
    {148, 150, 'V'},
    {150, 152, 'p'},
    {152, 153, 'O'},
    {153, 154, 'G'},
    {154, 155, 'U'},
    {155, 156, 'A'},
    {156, 165, 'K'},
    {165, 167, 'X'},
    {167, 168, 'H'},
    {168, 169, 'm'},
    {169, 171, '8'},
    {171, 173, 'R'},
    {173, 174, '#'},
    {174, 177, '$'},
    {177, 180, 'B'},
    {180, 181, 'g'},
    {181, 185, '0'},
    {185, 186, 'M'},
    {186, 187, 'N'},
    {187, 194, 'W'},
    {194, 200, 'Q'},
    {200, 205, '%'},
    {205, 225, '&'},
    {225, 255, '@'},
};

bool in_range(uint8_t x, uint8_t lb, uint8_t ub) {
    if (x >= lb && x < ub) 
        return true;
    return false;
}

char lumtochar(uint8_t lum) {
    for (int i = 0; i < sizeof(Lookup) / sizeof(range); i++) {
        if (in_range(lum, Lookup[i].min, Lookup[i].max))
            return Lookup[i].character;
    }

    if (lum == 255) {
        return '@';
    }

    return '~';
}


int blur_image(greyimage *src, greyimage *dest) {
    int kernelsize = 7;
    float kernel[][7] = {
        {0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067},
        {0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292},
        {0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117},
        {0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771},
        {0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117},
        {0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292},
        {0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067}
    };
    
    int offset_start = -1 * ((kernelsize - 1) / 2);
    int offset_end = ((kernelsize - 1) / 2);

    point cursor = {0};
    for (cursor.y = 0; cursor.y < src->imagedimensions.y; cursor.y++) {
        for (cursor.x = 0; cursor.x < src->imagedimensions.x; cursor.x++) {
            float sum = 0;
            for (int dy = offset_start; dy <= offset_end; dy++) {
                for (int dx = offset_start; dx <= offset_end; dx++) {
                    if (cursor.y + dy < src->imagedimensions.y && cursor.y + dy >= 0 && cursor.x + dx < src->imagedimensions.x && cursor.x + dx >= 0) {
                        sum += src->imagedata[cursor.y + dy][cursor.x + dx] * kernel[(kernelsize / 2) + dy][(kernelsize / 2) + dx];
                    }
                }
            }

            dest->imagedata[cursor.y][cursor.x] = round(sum);
        }
    }

    return 0;
}

