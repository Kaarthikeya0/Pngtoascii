#ifndef GETCHUNKDATA_H
#define GETCHUNKDATA_H

#include "structs.h"
#include <stdio.h>

struct imageHeader get_image_header(FILE *image);
void get_colour_palette(FILE *image, plteArray *colours);

#endif // !GETCHUNKDATA_H
