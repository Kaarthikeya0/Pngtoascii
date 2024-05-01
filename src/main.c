// C headers
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


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file.png\n", argv[0]);
        return 1;
    }

    FILE *image = fopen(argv[1], "r");
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

    char chunk_type[5];
    char iend[] = "IEND";
    while (strcmp(chun))
    uint32_t chunk_length = read_bytes_be32(image);
    fread(chunk_type, sizeof(char), 4, image);
    chunk_type[4] = 0;



    printf("Woo no errors\n");
    fclose(image);
    return EXIT_SUCCESS;
}

