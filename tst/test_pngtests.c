#include "../src/hdr/pngtests.h"
#include "../src/hdr/getchunkdata.h"
#include "../src/hdr/structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define COLOR_RESET "\e[0m"

int main(void)
{
    //test ispng
    FILE *firstwordfucked = fopen("tests/firstwordfucked.test", "r");
    if (firstwordfucked == NULL) {
        perror("bruh your file path is wrong somehow");
        return 1;
    }
    if (ispng(firstwordfucked)) {
        printf(RED "FAILED: bool ispng(FILE *file)\n" COLOR_RESET);
    }
    else {
        printf(GRN "SUCCESS: bool ispng(FILE *file)\n" COLOR_RESET);
    }

    //test has_ihdr
    FILE *chunktypefucked = fopen("tests/chunktypefucked.test", "r");
    if (chunktypefucked == NULL) {
        perror("bruh your file path is wrong somehow");
        return 1;
    }

    if (has_ihdr(chunktypefucked)) {
        printf(RED "FAILED: bool has_ihdr(FILE *file)\n" COLOR_RESET);
    }
    else {
        printf(GRN "SUCCESS: bool has_ihdr(FILE *file)\n" COLOR_RESET);
    }

    //test check_bit_depth_vs_colour_type(struct imageHeader hdr)
    FILE *bitdepthfucked = fopen("tests/bitdepthfucked.test", "r");
    if (bitdepthfucked == NULL) {
        perror("bruh your file path is wrong somehow");
        return 1;
    }

    struct imageHeader hdr = get_image_header(bitdepthfucked);
    if (check_bit_depth_vs_colour_type(hdr)) {
        printf(RED "FAILED: bool check_bit_depth_vs_colour_type(struct imageHeader hdr)\n" COLOR_RESET);
    }
    else {
        printf(GRN "SUCCESS: bool check_bit_depth_vs_colour_type(struct imageHeader hdr)\n" COLOR_RESET);
    }

    //test get_palette 
    FILE *paletteparsy = fopen("tests/plteparse2.test", "r");
    if (paletteparsy == NULL) {
        perror("bruh your file path is wrong somehow");
        return 1;
    }

    fseek(paletteparsy, 0x39, SEEK_SET);
    plteArray palette = get_colour_palette(paletteparsy, 2);
    char color1[7] = "AAAAAA";
    char color2[7] = "AAAAAA";
    char *colours[7] = {"EEFF22", "2266FF"};
    sprintf(color1, "%02X%02X%02X", (*palette.pltearray)[0].red,(*palette.pltearray)[0].green, (*palette.pltearray)[0].blue);
    sprintf(color2, "%02X%02X%02X", (*palette.pltearray)[1].red,(*palette.pltearray)[1].green, (*palette.pltearray)[1].blue);
    if (strcmp(color1, colours[0]) == 0 && strcmp(color2, colours[1]) == 0) {
        printf(GRN "SUCCESS: plteArray get_colour_palette(FILE *image, uint32_t nmemb)\n" COLOR_RESET);
    }
    else {
        printf(RED "FAILED: plteArray get_colour_palette(FILE *image, uint32_t nmemb)\n" COLOR_RESET);
        printf("COLOURS: %s, %s\n", color1, color2);
    }
    
    


    return EXIT_SUCCESS;
}
