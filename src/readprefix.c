// C headers
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// my headers
#include "gettingbyte.h"


// structs
struct imageHeader {
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t colour_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
};

bool ispng(FILE *file);
struct imageHeader get_image_header(FILE *image);
bool has_ihdr(FILE *file);


int main(int argc, char *argv[])
{
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
    (void) get_image_header(image);

    fclose(image);
    return EXIT_SUCCESS;
}

// Checks that the first 8 bytes are what they are supposed to be
// returns true if it is a PNG and false otherwise (rewinds stream)
bool ispng(FILE *file) {
    enum {
        header_length = 8,
        chunk_type_length = 4
    };

    uint8_t pngheader[header_length] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    rewind(file);
    uint8_t fileheader[header_length] = {0};
    fread(fileheader, sizeof(uint8_t), header_length, file);
    rewind(file);
    for (int i = 0; i < header_length; i++) {
        if (pngheader[i] != fileheader[i])
            return false;
    }


    return true;
}

// Checks that first chunk chunk in file is IHDR chunk
// returns true if found false otherwise (resets stream)
bool has_ihdr(FILE *file) {
    enum {
        header_length = 8,
        chunk_type_length = 4,
        chunk_length_length = 4
    };

    char chunk_type[] = "IHDR";
    char first_chunk_type[chunk_type_length + 1];

    fseek(file, header_length + chunk_length_length, SEEK_SET);
    fread(first_chunk_type, sizeof(char), chunk_type_length, file);
    rewind(file);
    first_chunk_type[chunk_type_length] = '\0';

    if (strcmp(chunk_type, first_chunk_type) != 0)
        return false;

    return true;
}

struct imageHeader get_image_header(FILE *image) {
    struct imageHeader hdr = {0};
    fseek(image, 16, SEEK_SET);
    hdr.width = read_bytes_be32(image);
    hdr.height = read_bytes_be32(image);
    hdr.bit_depth = read_byte(image);
    hdr.colour_type = read_byte(image);
    hdr.compression_method = read_byte(image);
    hdr.filter_method = read_byte(image);
    hdr.interlace_method = read_byte(image);
    printf("width: %d\nheight: %d\nbit depth: %d\ncolortype: %d\ncompression method: %d\nfiltermethod: %d\ninterlacemethod: %d\n", hdr.width, hdr.height, hdr.bit_depth, hdr.colour_type, hdr.compression_method, hdr.filter_method, hdr.interlace_method);
    
    return hdr;
}

