#include "hdr/gettingbyte.h"
#include "hdr/structs.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


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

// Returns a pointer to an array of triples that are in the PLTE chunk
// Requires the stream to be at the start of the chunk data and a nmemb (u32)
plteArray get_colour_palette(FILE *image, uint32_t nmemb) {
    fseek(image, 8, SEEK_CUR);

    // initialise struct and create array of triples
    plteArray colours = {.nmemb = nmemb};
    triple *triples = malloc(colours.nmemb * sizeof(triple));
    colours.pltearray = &triples;
    for (int i = 0; i < colours.nmemb; i++) {
        (*colours.pltearray)[i].red = read_byte(image);
        (*colours.pltearray)[i].green = read_byte(image);
        (*colours.pltearray)[i].blue = read_byte(image);
    }

    return colours;
}

