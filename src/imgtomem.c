#include "hdr/getchunkdata.h"
#include "hdr/gettingbyte.h"
#include "hdr/structs.h"
#include "hdr/imgtomem.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CHUNK 16384

int image_to_memory(struct imageHeader hdr, plteArray plt, FILE *imagefile, unsigned char **image_data, int *idata_size, unsigned char **uncompressed_image_data) {

    printf("*idata_size = %d\n", *idata_size);
    char chunk_type[] = "HONK";

    if (uncompressed_image_data == NULL) {
        perror("malloc fail");
        return 6;
    }

    char plte[] = "PLTE";
    char idat[] = "IDAT";
    char iend[] = "IEND";

    //main loop which runs through the file intil it hits a IEND chunk
    while (strcmp(chunk_type, iend) != 0) {
        uint32_t chunk_length = read_bytes_be32(imagefile);
        printf("chunk_length = %d\n", chunk_length);
        fread(chunk_type, sizeof(char), 4, imagefile);
        if (isupper(chunk_type[0])) {
            if (strcmp(chunk_type, plte) == 0) {
                if (plt.pltearray == NULL && !(hdr.colour_type == 0 || hdr.colour_type == 4)) {
                    if (chunk_length % 3 == 0) {
                        plt.nmemb = chunk_length / 3;
                        if (get_colour_palette(imagefile, &plt)) {
                            fprintf(stderr, "Reading PLTE chunk fail\n");
                            return 5;
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
                    printf("ftell(imagefile) = %ld\n", ftell(imagefile));
                    if (get_image_data(imagefile, image_data, idata_size, chunk_length)) {
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
            fseek(imagefile, chunk_length, SEEK_CUR);
        }

        // skip CRC (cant be bothered)
        fseek(imagefile, 4, SEEK_CUR);
    }

    return 0;
}
