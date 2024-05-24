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
#include "hdr/zlibtools.h"
#include "hdr/data_parse.h"
#include "hdr/toascii.h"
#include "hdr/handlemem.h"
#include "hdr/imgtomem.h"

#define CHUNK 16384

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file.png\n", argv[0]);
        return 1;
    }

    FILE *imagefile = fopen(argv[1], "rb");
    if (imagefile == NULL) {
        perror("Image open failed");
        return 2;
    }

    // checks if the program is capable of using the given image
    bool valid_png = ispng(imagefile);
    bool has_header = has_ihdr(imagefile);
    if (!valid_png) {
        fprintf(stderr, "The file provided does not start with the appropriate 8 bytes\n");
        return 3;
    }
    if (!has_header) {
        fprintf(stderr, "The image provided does not contain the IHDR chunk\n");
        return 3;
    }

    fseek(imagefile, 8, SEEK_SET);
    struct imageHeader hdr = get_image_header(imagefile);
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

    if (hdr.bit_depth < 8 || hdr.colour_type == 3) {
        fprintf(stderr, "Not implemented yet!\n");
        return 5;
    }

    // skip CRC for IHDR 
    fseek(imagefile, 4, SEEK_CUR);

    // this chunk of code extracts the IDAT data and the plt data from the png image
    // it relies on the hdr struct filled earlier and the imagefile
    // it returns the plt data and the image data
    unsigned char *image_data = NULL;
    unsigned char *uncompressed_image_data = malloc(CHUNK);
    int uncompressed_data_total_size = CHUNK;
    int uncompressed_data_used_data = 0;
    plteArray plt = {.nmemb = 0, .pltearray = NULL};

    int idata_size = 0;

    if (image_to_memory(hdr, plt, imagefile, &image_data, &idata_size, &uncompressed_image_data)) {
        perror("Cry");
        return 4;
    }


    if (decompress(image_data, idata_size, &uncompressed_image_data, &uncompressed_data_total_size, &uncompressed_data_used_data))
        return 7;

    image ogimage;
    if (constructimage(&ogimage, (point) {.x = hdr.width, .y = hdr.height})) {
        return 5;
    }
    

    if (data_to_struct(hdr, &ogimage, uncompressed_image_data, uncompressed_data_used_data)) {
        fprintf(stderr, "Something went wrong\n");
        return 4;
    }


    greyimage lumvalues;
    if (constructgreyimage(&lumvalues, (point) {.x = hdr.width, .y = hdr.height}))
        return 5;
    
    
    point cursor = {0};
    for (cursor.y = 0; cursor.y < ogimage.imagedimensions.y; cursor.y++) {
        for (cursor.x = 0; cursor.x < ogimage.imagedimensions.x; cursor.x++) {
            float pixel_lumvalue = (float) (ogimage.imagedata[cursor.y][cursor.x].red + ogimage.imagedata[cursor.y][cursor.x].green + ogimage.imagedata[cursor.y][cursor.x].blue) / 3;
            lumvalues.imagedata[cursor.y][cursor.x] = UINT8_MAX - pixel_lumvalue;
        }
    }


    greyimage blurimage;
    if (constructgreyimage(&blurimage, lumvalues.imagedimensions))
        return 5;

    if (blur_image(&lumvalues, &blurimage)) {
        perror("Blur image failed");
        return 6;
    }

        

    int target_rowlen = 300;
    int scale = lumvalues.imagedimensions.x / (target_rowlen / 2);
    if (blurimage.imagedimensions.x < target_rowlen) {
        scale = 1;
    }

    for (cursor.y = 0; cursor.y < lumvalues.imagedimensions.y; cursor.y += scale) {
        for (cursor.x = 0; cursor.x < lumvalues.imagedimensions.x; cursor.x += scale) {
            uint8_t lum = blurimage.imagedata[cursor.y][cursor.x];
            putchar(lumtochar(lum));
            putchar(lumtochar(lum));
        }

        putchar(10);
    }


    freegreyimage(&lumvalues);
    freegreyimage(&blurimage);
    freeimage(&ogimage);
    free(uncompressed_image_data);
    free(image_data);
    free(plt.pltearray);
    fclose(imagefile);
    return EXIT_SUCCESS;
}
