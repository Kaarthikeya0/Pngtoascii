#ifndef PNGTESTS_H
#define PNGTESTS_H 

#include <stdio.h>
#include <stdbool.h>

bool ispng(FILE *file);
struct imageHeader get_image_header(FILE *image);
bool has_ihdr(FILE *file);
bool check_bit_depth_vs_colour_type(struct imageHeader hdr);

#endif // !PNGTESTS_H

