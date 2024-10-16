#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hdr/data_parse.h"
#include "hdr/structs.h"

int data_to_struct(struct imageHeader hdr, plteArray plt, image *dst, unsigned char *uncompressed_image_data, int uncompressed_data_size) {

    int scanline_len = uncompressed_data_size / hdr.height;
    int bits_per_pixel = ((scanline_len - 1) * 8) / hdr.width;
    int bytes_per_pixel = (scanline_len - 1) / hdr.width;

    unsigned char prev_scanline[scanline_len];
    unsigned char curr_scanline[scanline_len];
    for (int scanline_num = 0; scanline_num < hdr.height; scanline_num++) {
        if (scanline_num != 0) {
            memcpy(prev_scanline, curr_scanline, scanline_len * sizeof(unsigned char));
        }
        memcpy(curr_scanline, uncompressed_image_data + scanline_num * scanline_len, scanline_len * sizeof(unsigned char));

        int filter_type = recon_scanline(curr_scanline, prev_scanline, scanline_len, scanline_num, bits_per_pixel, bytes_per_pixel);

        if (filter_type == 9) {
            return filter_type;
        }

        unsigned char scanline[scanline_len - 1];
        int byte_number = 0;
        memcpy(scanline, curr_scanline + 1, scanline_len - 1);
        if (hdr.bit_depth < 8 && hdr.colour_type == 0) {
            int bit_count = 0;
            uint8_t buff;
            for (int i = 0; i < scanline_len - 1; i++) {
                buff = scanline[byte_number++];
                for (int j = 8 - hdr.bit_depth; j >= 0 && bit_count < hdr.width; j -= hdr.bit_depth, bit_count++) {
                    uint8_t value = (buff >> j) & ((1 << hdr.bit_depth) - 1);
                    uint8_t mapped_value = (value * UINT8_MAX) / ((1 << hdr.bit_depth) - 1);
                    dst->imagedata[scanline_num][bit_count].red = mapped_value;
                    dst->imagedata[scanline_num][bit_count].green = mapped_value;
                    dst->imagedata[scanline_num][bit_count].blue = mapped_value;
                    dst->imagedata[scanline_num][bit_count].alpha = UINT8_MAX;
                }
            }
        }
        else if (hdr.bit_depth < 8 && hdr.colour_type == 3) {
            int bit_count = 0;
            uint8_t buff;
            for (int i = 0; i < scanline_len - 1; i++) {
                buff = scanline[byte_number++];
                for (int j = 8 - hdr.bit_depth; j >= 0 && bit_count < hdr.width; j -= hdr.bit_depth, bit_count++) {
                    uint8_t value = (buff >> j) & ((1 << hdr.bit_depth) - 1);
                    dst->imagedata[scanline_num][bit_count] = plt.pltearray[value];
                }
            }
        }
        else if (hdr.bit_depth == 8 && hdr.colour_type == 0) {
            uint8_t buff;
            for (int i = 0; i < hdr.width; i++) {
                buff = scanline[byte_number++];
                dst->imagedata[scanline_num][i].red = buff;
                dst->imagedata[scanline_num][i].green = buff;
                dst->imagedata[scanline_num][i].blue = buff;
                dst->imagedata[scanline_num][i].alpha = UINT8_MAX;
            }
        }
        else if (hdr.bit_depth == 8 && hdr.colour_type == 2) {
            for (int i = 0; i < hdr.width; i++) {
                dst->imagedata[scanline_num][i].red = scanline[byte_number++];
                dst->imagedata[scanline_num][i].green = scanline[byte_number++];
                dst->imagedata[scanline_num][i].blue = scanline[byte_number++];
                dst->imagedata[scanline_num][i].alpha = UINT8_MAX;
            }
        }
        else if (hdr.bit_depth == 8 && hdr.colour_type == 3) {
            uint8_t buff;
            for (int i = 0; i < hdr.width; i++) {
                buff = scanline[byte_number++];
                dst->imagedata[scanline_num][i] = plt.pltearray[buff];
            }
        }
        else if (hdr.bit_depth == 8 && hdr.colour_type == 4) {
            uint8_t buff;
            for (int i = 0; i < hdr.width; i++) {
                buff = scanline[byte_number++];
                dst->imagedata[scanline_num][i].red = buff;
                dst->imagedata[scanline_num][i].green = buff;
                dst->imagedata[scanline_num][i].blue = buff;
                dst->imagedata[scanline_num][i].alpha = scanline[byte_number++];
            }
        }
        else if (hdr.bit_depth == 8 && hdr.colour_type == 6) {
            for (int i = 0; i < hdr.width; i++) {
                dst->imagedata[scanline_num][i].red = scanline[byte_number++];
                dst->imagedata[scanline_num][i].green = scanline[byte_number++];
                dst->imagedata[scanline_num][i].blue = scanline[byte_number++];
                dst->imagedata[scanline_num][i].alpha = scanline[byte_number++];
            }
        }
        else if (hdr.bit_depth == 16 && hdr.colour_type == 0) {
            uint16_t buff;
            for (int i = 0; i < hdr.width; i++) {
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].red = (0xFF *  buff) / 0xFFFF;
                dst->imagedata[scanline_num][i].green = (0xFF *  buff) / 0xFFFF;
                dst->imagedata[scanline_num][i].blue = (0xFF *  buff) / 0xFFFF;
                dst->imagedata[scanline_num][i].alpha = UINT8_MAX;
            }
        }
        else if (hdr.bit_depth == 16 && hdr.colour_type == 2) {
            uint16_t buff;
            for (int i = 0; i < hdr.width; i++) {
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].red = (0xFF *  buff) / 0xFFFF;
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].green = (0xFF *  buff) / 0xFFFF;
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].blue = (0xFF *  buff) / 0xFFFF;
                dst->imagedata[scanline_num][i].alpha = UINT8_MAX;
            }
        }
        else if (hdr.bit_depth == 16 && hdr.colour_type == 4) {
            uint16_t buff;
            for (int i = 0; i < hdr.width; i++) {
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].red = (0xFF *  buff) / 0xFFFF;
                dst->imagedata[scanline_num][i].green = (0xFF *  buff) / 0xFFFF;
                dst->imagedata[scanline_num][i].blue = (0xFF *  buff) / 0xFFFF;
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].alpha = (0xFF *  buff) / 0xFFFF;
            }
        }
        else if (hdr.bit_depth == 16 && hdr.colour_type == 6) {
            uint16_t buff;
            for (int i = 0; i < hdr.width; i++) {
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].red = (0xFF *  buff) / 0xFFFF;
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].green = (0xFF *  buff) / 0xFFFF;
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].blue = (0xFF *  buff) / 0xFFFF;
                buff = scanline[byte_number++];
                buff = (buff << 8) + scanline[byte_number++];
                dst->imagedata[scanline_num][i].alpha = (0xFF *  buff) / 0xFFFF;
            }
        }
    }

    return 0;
}


int recon_scanline(unsigned char *curr_scanline, unsigned char *prev_scanline, int scanline_len, int scanline_num, int bits_per_pixel, int bytes_per_pixel) {
    uint8_t filter_type = curr_scanline[0];
    if (bits_per_pixel < 8) {
        bytes_per_pixel = 1;
    }
    switch (filter_type) {
        case 0:
            break;
        case 1:
            for (int i = 1; i < scanline_len; i++) {
                int a = i > 1 ? curr_scanline[i - bytes_per_pixel] : 0;
                curr_scanline[i] += a;
            }
            break;
        case 2:
            for (int i = 1; i < scanline_len; i++) {
                int b = scanline_num > 0 ? prev_scanline[i] : 0;
                curr_scanline[i] += b;
            }
            break;
        case 3:
            for (int i = 1; i < scanline_len; i++) {
                int a = i > 1 ? curr_scanline[i - bytes_per_pixel] : 0;
                int b = scanline_num > 0 ? prev_scanline[i] : 0;
                curr_scanline[i] = ((uint16_t) curr_scanline[i] + ((a + b) / 2)) % 256;
            }
            break;
        case 4:
            for (int i = 1; i < scanline_len; i++) {
                int a = i > 1 ? curr_scanline[i - bytes_per_pixel] : 0;
                int b = scanline_num > 0 ? prev_scanline[i] : 0;
                int c = scanline_num > 0 && i > 1 ? prev_scanline[i - bytes_per_pixel] : 0;
                curr_scanline[i] = ((uint16_t) curr_scanline[i] + paeth_predictor(a, b, c)) % 256;
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
