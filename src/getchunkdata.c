#include "hdr/gettingbyte.h"
#include "hdr/structs.h"
#include "hdr/getchunkdata.h"

#include <stdio.h>
#include <stdlib.h>


// Returns a struct containing the details stored in IHDR, assumes that IHDR exists and seeks to byte number 16 (header: 8, length number: 4, chunk code: 4)
struct imageHeader get_image_header(FILE *imagefile) {
    struct imageHeader hdr = {0};
    fseek(imagefile, 16, SEEK_SET);
    hdr.width = read_bytes_be32(imagefile);
    hdr.height = read_bytes_be32(imagefile);
    hdr.bit_depth = read_byte(imagefile);
    hdr.colour_type = read_byte(imagefile);
    hdr.compression_method = read_byte(imagefile);
    hdr.filter_method = read_byte(imagefile);
    hdr.interlace_method = read_byte(imagefile);
    
    return hdr;
}

// Returns a array of triples that are in the PLTE chunk
// Requires the stream to be at the start of the chunk data and a nmemb (u32)
int get_colour_palette(FILE *imagefile, plteArray *colours) {
    // initialise struct and create array of triples
    colours->pltearray = malloc(colours->nmemb * sizeof(triple));
    if (colours->pltearray == NULL) {
        return 1;
    }
    for (int i = 0; i < colours->nmemb; i++) {
        (colours->pltearray)[i].red = read_byte(imagefile);
        (colours->pltearray)[i].green = read_byte(imagefile);
        (colours->pltearray)[i].blue = read_byte(imagefile);
    }
    return 0;
}

int get_image_data(FILE *imagefile, unsigned char **ptr_to_image_data, int *ptr_to_idata_size, int chunk_length) {
    if (*ptr_to_image_data == NULL)
        *ptr_to_image_data= malloc(chunk_length * sizeof(unsigned char));
    else
        *ptr_to_image_data = realloc(*ptr_to_image_data, (chunk_length + *ptr_to_idata_size) * sizeof(unsigned char));

    if (ptr_to_image_data == NULL) {
        perror("Reallocation failed");
        return 1;
    }

    fread(*ptr_to_image_data + *ptr_to_idata_size, chunk_length * sizeof(unsigned char), 1, imagefile);
    *ptr_to_idata_size += chunk_length;
    return 0;
}

