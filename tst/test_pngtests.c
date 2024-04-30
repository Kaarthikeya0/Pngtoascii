#include "../src/hdr/pngtests.h"
#include "../src/hdr/structs.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    //test ispng
    FILE *firstwordfucked = fopen("tests/firstwordfucked.test", "r");
    if (firstwordfucked == NULL) {
        perror("bruh your file path is wrong somehow");
        return 1;
    }
    if (ispng(firstwordfucked)) {
        printf("FAILED: bool ispng(FILE *file)\n");
    }
    else {
        printf("SUCCESS: bool ispng(FILE *file)\n");
    }

    //test has_ihdr
    FILE *chunktypefucked = fopen("tests/chunktypefucked.test", "r");
    if (chunktypefucked == NULL) {
        perror("bruh your file path is wrong somehow");
        return 1;
    }
    if (has_ihdr(chunktypefucked)) {
        printf("FAILED: bool has_ihdr(FILE *file)\n");
    }
    else {
        printf("SUCCESS: bool has_ihdr(FILE *file)\n");
    }

    //test check_bit_depth_vs_colour_type(struct imageHeader hdr)
    FILE *bitdepthfucked = fopen("tests/bitdepthfucked.test", "r");
    if (bitdepthfucked == NULL) {
        perror("bruh your file path is wrong somehow");
        return 1;
    }

    struct imageHeader hdr = get_image_header(bitdepthfucked);
    if (check_bit_depth_vs_colour_type(hdr)) {
        printf("FAILED: bool check_bit_depth_vs_colour_type(struct imageHeader hdr)\n");
    }
    else {
        printf("SUCCESS: bool check_bit_depth_vs_colour_type(struct imageHeader hdr)\n");
    }
    


    return EXIT_SUCCESS;
}
