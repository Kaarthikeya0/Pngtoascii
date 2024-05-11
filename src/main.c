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
    bool valid_colour_bit_combination = check_bit_depth_vs_colour_type(hdr);
    if (!valid_colour_bit_combination) {
        fprintf(stderr, "The image provided does not have the correct combination of bit depth and colour type\n");
        return 4;
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
                if (plt.pltearray == NULL) {
                    if (chunk_length % 3 == 0) {
                        plt.nmemb = chunk_length / 3;
                        if (get_colour_palette(image, &plt)) {
                            fprintf(stderr, "Reading PLTE chunk fail\n");
                            return 5;
                        }
                    }
                }
                else {
                    fprintf(stderr, "More than one PLTE chunk detected, stopping.\n");
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

    printf("Compressed read: \n");
    for (int i = 0; i < idata_size; i++) {
        printf("%02X ", image_data[i]);
        if ((i + 1) % 8 == 0) {
            putchar(10);
        }
    }
    putchar(10);

    if (decompress(image_data, idata_size, &uncompressed_image_data, &uncompressed_data_total_size, &uncompressed_data_used_data))
        return 7;

    printf("uncompressed_data_used_data = %d\n", uncompressed_data_used_data);
    printf("Uncompressed read: \n");
    for (int i = 0; i < uncompressed_data_used_data; i++) {
        printf("%02X ", uncompressed_image_data[i]);
        if ((i + 1) % 8 == 0) {
            putchar(10);
        }
    }
    putchar(10);

    free(uncompressed_image_data);
    free(image_data);
    free(plt.pltearray);
    fclose(image);
    printf("Woo no errors\n");
    return EXIT_SUCCESS;
}
