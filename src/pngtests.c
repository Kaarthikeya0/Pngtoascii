#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "hdr/structs.h"
#include "hdr/gettingbyte.h"

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

// Checks that first chunk chunk in file is IHDR chunk and the chunk length is 13
// returns true if found false otherwise (resets stream)
bool has_ihdr(FILE *file) {
    enum {
        header_length = 8,
        chunk_type_length = 4,
        chunk_length_length = 4
    };

    char chunk_type[] = "IHDR";
    char first_chunk_type[chunk_type_length + 1];

    fseek(file, header_length, SEEK_SET);
    int32_t first_chunk_length = read_bytes_be32(file);


    fseek(file, header_length + chunk_length_length, SEEK_SET);
    fread(first_chunk_type, sizeof(char), chunk_type_length, file);
    rewind(file);
    first_chunk_type[chunk_type_length] = '\0';

    if (strcmp(chunk_type, first_chunk_type) != 0 && first_chunk_length == 13)
        return false;

    return true;
}

// Returns a struct containing the details stored in IHDR, assumes that IHDR exists and seeks to byte number 16 (header: 8, length number: 4, chunk code: 4)
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
    
    return hdr;
}

// Returns a true or false value depending on whether the bit depth and colour type are compatible
// false if incompatible OR colour type doesnt exist
// true if compatible
bool check_bit_depth_vs_colour_type(struct imageHeader hdr) {
    bool flag = false;
    switch (hdr.colour_type) {
        case 0: {
            int allowed_bitdepth[] = {1, 2, 4, 8, 16};
            for (int i = 0; i < sizeof(allowed_bitdepth) / sizeof(int); i++) {
                if (allowed_bitdepth[i] == hdr.bit_depth)
                    flag = true;
            }
            break;
        }
        case 2: {
            int allowed_bitdepth[] = {8, 16};
            for (int i = 0; i < sizeof(allowed_bitdepth) / sizeof(int); i++) {
                if (allowed_bitdepth[i] == hdr.bit_depth)
                    flag = true;
            }
            break;
        }

        case 3: {
            int allowed_bitdepth[] = {1, 2, 4, 8};
            for (int i = 0; i < sizeof(allowed_bitdepth) / sizeof(int); i++) {
                if (allowed_bitdepth[i] == hdr.bit_depth)
                    flag = true;
            }
            break;
        }

        case 4: {
            int allowed_bitdepth[] = {8, 16};
            for (int i = 0; i < sizeof(allowed_bitdepth) / sizeof(int); i++) {
                if (allowed_bitdepth[i] == hdr.bit_depth)
                    flag = true;
            }
            break;
        }

        case 6: {
            int allowed_bitdepth[] = {8, 16};
            for (int i = 0; i < sizeof(allowed_bitdepth) / sizeof(int); i++) {
                if (allowed_bitdepth[i] == hdr.bit_depth)
                    flag = true;
            }
            break;
        }

        default:
            flag = false;
    }

    return flag;
}

