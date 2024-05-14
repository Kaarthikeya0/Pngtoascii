#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "hdr/data_parse.h"

int recon_scanline(unsigned char *curr_scanline, unsigned char *prev_scanline, int scanline_len, int scanline_num, int bits_per_pixel, int bytes_per_pixel) {
    uint8_t filter_type = curr_scanline[0];
    switch (filter_type) {
        case 0:
            break;
        case 1:
            for (int i = 1; i < scanline_len; i++) {
                if (bits_per_pixel < 8) {
                    if (i != 1) {
                        curr_scanline[i] += curr_scanline[i - 1];
                    }
                }
                else {
                    if (i > bytes_per_pixel) {
                        curr_scanline[i] += curr_scanline[i - bytes_per_pixel];
                    }
                }
            }
            break;
        case 2:
            if (scanline_num != 0) {
                for (int i = 1; i < scanline_len; i++) {
                    curr_scanline[i] += prev_scanline[i];
                }
            }
            break;
        case 3:
            if (scanline_num != 0) {
                for (int i = 1; i < scanline_len; i++) {
                    if (i != 1) {
                        curr_scanline[i] += (prev_scanline[i] + curr_scanline[i - 1]) / 2;
                    }
                }
            }
            break;
        case 4:
            if (scanline_num != 0) {
                for (int i = 1; i < scanline_len; i++) {
                    if (i != 1) {
                        curr_scanline[i] += paeth_predictor(curr_scanline[i - 1], prev_scanline[i], prev_scanline[i - 1]);
                    }
                }
            }
            break;
        default:
            fprintf(stderr, "Filter type %d is not valid in this program\n", filter_type);
            return 9;
    }

    return filter_type;
}
int paeth_predictor(int a, int b, int c) {
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);
    if (pa <= pb && pa <= pc)
        return a;
    else if (pb <= pc)
        return b;
    else
        return c;
}

