// C headers
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// my headers
#include "hdr/pngtests.h"
#include "hdr/structs.h"
#include "hdr/getchunkdata.h"
#include "hdr/zlibtools.h"
#include "hdr/data_parse.h"
#include "hdr/toascii.h"
#include "hdr/handlemem.h"
#include "hdr/imgtomem.h"
#include "hdr/imgtostring.h"

#define CHUNK 16384

int pngtostring(char *pngfile,struct coloured_string *dest, int target_rowlen, bool blur, bool switch_chars, bool square) {

    FILE *imagefile = fopen(pngfile, "rb");

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

    if (image_to_memory(hdr, &plt, imagefile, &image_data, &idata_size, &uncompressed_image_data)) {
        perror("Cry");
        return 4;
    }

    if (decompress(image_data, idata_size, &uncompressed_image_data, &uncompressed_data_total_size, &uncompressed_data_used_data))
        return 7;

    image ogimage;
    if (constructimage(&ogimage, (point) {.x = hdr.width, .y = hdr.height})) {
        return 5;
    }
    

    if (data_to_struct(hdr, plt, &ogimage, uncompressed_image_data, uncompressed_data_used_data)) {
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
            //float pixel_lumvalue = 0.299 * ogimage.imagedata[cursor.y][cursor.x].red + 0.587 * ogimage.imagedata[cursor.y][cursor.x].green + 0.114 * ogimage.imagedata[cursor.y][cursor.x].blue;
            if (switch_chars)
                lumvalues.imagedata[cursor.y][cursor.x] = UINT8_MAX - pixel_lumvalue;
            else
                lumvalues.imagedata[cursor.y][cursor.x] = pixel_lumvalue;

        }
    }



    greyimage blurimage;
    image rgblurimage;
    if (blur) {
        if (constructgreyimage(&blurimage, lumvalues.imagedimensions))
            return 5;

        if (constructimage(&rgblurimage, lumvalues.imagedimensions))
            return 5;

        if (blur_image_rgb(&ogimage, &rgblurimage)) {
            perror("Blur image failed");
            return 6;
        }
        
        for (cursor.y = 0; cursor.y < ogimage.imagedimensions.y; cursor.y++) {
            for (cursor.x = 0; cursor.x < ogimage.imagedimensions.x; cursor.x++) {
                triple pixel = rgblurimage.imagedata[cursor.y][cursor.x];
                float pixel_lumvalue = 0.299 * pixel.red + 0.587 * pixel.green + 0.114 * pixel.blue;
                if (switch_chars)
                    blurimage.imagedata[cursor.y][cursor.x] = UINT8_MAX - pixel_lumvalue;
                else
                    blurimage.imagedata[cursor.y][cursor.x] = pixel_lumvalue;

            }
        }
    }

        

    int scale = square ? lumvalues.imagedimensions.x / target_rowlen : lumvalues.imagedimensions.x / (target_rowlen / 2);
    if (scale == 0) scale = 1;

    int counter = 0;
    point counting = {0};
    for (cursor.y = 0; cursor.y < lumvalues.imagedimensions.y; cursor.y += scale) {
        for (cursor.x = 0; cursor.x < lumvalues.imagedimensions.x; cursor.x += scale) {
            counter += square ? 1 : 2;
        }
        counter++;
    }

    for (cursor.x = 0; cursor.x < lumvalues.imagedimensions.x; cursor.x += scale) counting.x += square ? 1 : 2;
    for (cursor.y = 0; cursor.y < lumvalues.imagedimensions.y; cursor.y += scale) counting.y += 1;

    dest->str = calloc((counter + 1), sizeof(struct coloured_string));
    dest->str[counter].character = '\0';
    dest->dimensions = counting;

    struct coloured_char *destination = dest->str;
    int dest_byte_cursor = 0;
    for (cursor.y = 0; cursor.y < lumvalues.imagedimensions.y; cursor.y += scale) {
        for (cursor.x = 0; cursor.x < lumvalues.imagedimensions.x; cursor.x += scale) {
            uint8_t lum = blur ? blurimage.imagedata[cursor.y][cursor.x] : lumvalues.imagedata[cursor.y][cursor.x];
            char c = lumtochar(lum);
            destination[dest_byte_cursor].character = c;
            destination[dest_byte_cursor++].colour = blur ? rgblurimage.imagedata[cursor.y][cursor.x] : ogimage.imagedata[cursor.y][cursor.x];
            if (!square) {
                destination[dest_byte_cursor].character = c;
                destination[dest_byte_cursor++].colour = blur ? rgblurimage.imagedata[cursor.y][cursor.x] : ogimage.imagedata[cursor.y][cursor.x];
            }
        }
        destination[dest_byte_cursor].character = '\n';
        destination[dest_byte_cursor++].colour = (triple) {.red = 0, .green = 0, .blue = 0, .alpha = 0};
    }

    freegreyimage(&lumvalues);
    if (blur) freegreyimage(&blurimage);
    freeimage(&ogimage);
    free(uncompressed_image_data);
    free(image_data);
    free(plt.pltearray);
    fclose(imagefile);
    return EXIT_SUCCESS;
}
