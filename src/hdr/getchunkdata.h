#ifndef GETCHUNKDATA_H
#define GETCHUNKDATA_H

#include "structs.h"
#include <stdio.h>

struct imageHeader get_image_header(FILE *image);
int get_colour_palette(FILE *image, plteArray *colours);
int get_image_data(FILE *image, unsigned char **ptr_to_image_data, int *ptr_to_idata_size, int chunk_length);

#endif // !GETCHUNKDATA_H
