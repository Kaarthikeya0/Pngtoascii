// C headers
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

// my headers
#include "hdr/pngtests.h"
#include "hdr/structs.h"
#include "hdr/getchunkdata.h"
#include "hdr/gettingbyte.h"
#include "hdr/zlibtools.h"
#include "hdr/data_parse.h"

#define CHUNK 16384

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file.png\n", argv[0]);
        return 1;
    }

    FILE *image = fopen(argv[1], "rb");
    if (image == NULL) {
        perror("Image open failed");
        return 2;
    }

    bool valid_png = ispng(image);
    bool has_header = has_ihdr(image);
    if (!valid_png) {
        fprintf(stderr, "The file provided does not start with the appropriate 8 bytes\n");
        return 3;
    }
    if (!has_header) {
        fprintf(stderr, "The image provided does not contain the IHDR chunk\n");
        return 3;
    }

    //skip to start of length of first chunk
    fseek(image, 8, SEEK_SET);
    struct imageHeader hdr = get_image_header(image);
    printf("Width: %d\nHeight: %d\nBit Depth: %d\nColour Type: %d\nFilter Method: %d\nInterlace Method: %d\nCompression Method: %d\n", hdr.width, hdr.height, hdr.bit_depth, hdr.colour_type, hdr.filter_method, hdr.interlace_method, hdr.compression_method);
    bool valid_colour_bit_combination = check_bit_depth_vs_colour_type(hdr);
    if (!valid_colour_bit_combination) {
        fprintf(stderr, "The image provided does not have the correct combination of bit depth and colour type\n");
        return 4;
    }

    
    if (hdr.compression_method != 0) {
        fprintf(stderr, "Invalid compression method\n");
        return 4;
    }

    if (hdr.filter_method != 0) {
        fprintf(stderr, "Invalid filter method\n");
        return 4;
    }

    if (hdr.interlace_method != 0) {
        fprintf(stderr, "Cant be fucked to do interlaced images, bye!\n");
        return 5;
    }

    if (hdr.bit_depth < 8) {
        fprintf(stderr, "Not implemented yet!\n");
        return 5;
    }

    // skip CRC for IHDR (cannot be bothered atm)
    fseek(image, 4, SEEK_CUR);

    // now we are at the first chunk of the file that isnt IHDR
    // get the length of the next chunk

    char chunk_type[5] = "HONK";
    char plte[] = "PLTE";
    char idat[] = "IDAT";
    unsigned char *image_data = NULL;
    unsigned char *uncompressed_image_data = malloc(CHUNK);
    int uncompressed_data_total_size = CHUNK;
    int uncompressed_data_used_data = 0;

    if (uncompressed_image_data == NULL) {
        perror("malloc fail");
        return 6;
    }
    int idata_size = 0;
    char iend[] = "IEND";


    plteArray plt;
    plt.nmemb = 0;
    plt.pltearray = NULL;

    while (strcmp(chunk_type, iend) != 0) {
        uint32_t chunk_length = read_bytes_be32(image);
        fread(chunk_type, sizeof(char), 4, image);
        if (isupper(chunk_type[0])) {

            if (strcmp(chunk_type, plte) == 0) {
                if (plt.pltearray == NULL && !(hdr.colour_type == 0 || hdr.colour_type == 4)) {
                    if (chunk_length % 3 == 0) {
                        plt.nmemb = chunk_length / 3;
                        if (get_colour_palette(image, &plt)) {
                            fprintf(stderr, "Reading PLTE chunk fail\n");
                            return 5;
                        }
                        for (int i = 0; i < plt.nmemb; i++) {
                            printf("Colour %02X: %02X%02X%02X", i + 1, plt.pltearray[i].red, plt.pltearray[i].green, plt.pltearray[i].blue);
                            putchar(10);
                        }
                    }
                }
                else {
                    fprintf(stderr, "PLTE chunk invalid\n");
                    return 5;
                }
            }
            else if (strcmp(chunk_type, idat) == 0) {
                if (hdr.colour_type == 3 && plt.pltearray == NULL) {
                    fprintf(stderr, "PLTE chunk couldnt be found before first IDAT chunk in colour type 3\n");
                    return 5;
                }
                else {
                    printf("data chunk of %d bytes\n", chunk_length);
                    if (get_image_data(image, &image_data, &idata_size, chunk_length)) {
                        return 6;
                    }
                }
            }
            else if (strcmp(chunk_type, iend) == 0){
                continue;
            }
            else {
                fprintf(stderr, "Unrecognised critical chunk %s found\n", chunk_type);
                return 6;
            }
        }
        else {
            fseek(image, chunk_length, SEEK_CUR);
        }

        // skip CRC (cant be bothered)
        fseek(image, 4, SEEK_CUR);
    }

    if (decompress(image_data, idata_size, &uncompressed_image_data, &uncompressed_data_total_size, &uncompressed_data_used_data))
        return 7;

    point ImageSize = {.x = hdr.width, .y = hdr.height};
    triple Image[ImageSize.y][ImageSize.y];
    int scanline_len = uncompressed_data_used_data / hdr.height;
    int bits_per_pixel = ((scanline_len - 1) * 8) / hdr.width;
    int bytes_per_pixel = (scanline_len - 1) / hdr.width;

    printf("scanline_len = %d\n", scanline_len);
    printf("bits_per_pixel = %d\n", bits_per_pixel);
    printf("bytes_per_pixel = %d\n", bytes_per_pixel);
    printf("Uncompressed read: \n");

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

        FILE *scanline = fmemopen(curr_scanline + 1, sizeof(unsigned char) * scanline_len, "r");
        if (hdr.bit_depth == 8 && hdr.colour_type == 2) {
            for (int i = 0; i < hdr.width; i++) {
                fread(&Image[scanline_num][i].red, sizeof(uint8_t), 1, scanline);
                fread(&Image[scanline_num][i].green, sizeof(uint8_t), 1, scanline);
                fread(&Image[scanline_num][i].blue, sizeof(uint8_t), 1, scanline);
            }
        }
        else if (hdr.bit_depth == 16 && hdr.colour_type == 2) {
            for (int i = 0; i < hdr.width; i++) {
                uint16_t buff;
                fread(&buff, sizeof(buff), 1, scanline);
                Image[scanline_num][i].red = (0xFF * buff) / 0xFFFF;
                fread(&buff, sizeof(buff), 1, scanline);
                Image[scanline_num][i].green = (0xFF * buff) / 0xFFFF;
                fread(&buff, sizeof(buff), 1, scanline);
                Image[scanline_num][i].blue = (0xFF * buff) / 0xFFFF;
            }
        }

        printf("From them scanline data: ");
        for (int i = 0; i < scanline_len; i++) {
            if (i == 0)
                continue;
            int curr_byte = curr_scanline[i];
            printf("%02X ", curr_byte);
        }
        putchar(10);

        printf("From them triples:       ");
        for (int i = 0; i < hdr.width; i++) {
            printf("%02X %02X %02X ", Image[scanline_num][i].red, Image[scanline_num][i].green, Image[scanline_num][i].blue);
        }
        putchar(10);

        fclose(scanline);
    }

    free(uncompressed_image_data);
    free(image_data);
    free(plt.pltearray);
    fclose(image);
    printf("Woo no errors\n");
    return EXIT_SUCCESS;
}
