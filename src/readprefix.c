#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>


typedef struct chunkprop {
    uint8_t chunk_length;
    char chunk_type[4];
} chunk_properties;

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

uint32_t read_bytes_be32(FILE *file);
uint8_t read_byte(FILE *file);

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
    if (valid_png) {
        //skip to start of length of first chunk
        fseek(image, 8, SEEK_SET);
        (void) get_image_header(image);
    }

    fclose(image);
    return EXIT_SUCCESS;
}

// Checks that the first 8 bytes are what they are supposed to be and the first chunk is IHDR, 
// returns true if it is a PNG and false otherwise (rewinds stream)
bool ispng(FILE *file) {
    enum {
        header_length = 8,
        chunk_type_length = 4
    };

    uint8_t pngheader[header_length] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    char chunk_type[] = "IHDR";
    rewind(file);
    uint8_t fileheader[header_length] = {0};
    char first_chunk_type[chunk_type_length + 1];
    fread(fileheader, sizeof(uint8_t), header_length, file);
    for (int i = 0; i < header_length; i++) {
        if (pngheader[i] != fileheader[i])
            return false;
    }
    fseek(file, 4, SEEK_CUR);
    fread(first_chunk_type, sizeof(char), chunk_type_length, file);
    first_chunk_type[chunk_type_length] = '\0';

    if (strcmp(chunk_type, first_chunk_type) != 0)
        return false;
    rewind(file);


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
