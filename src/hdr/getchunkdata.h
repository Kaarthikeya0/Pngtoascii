#ifndef GETCHUNKDATA_H
#define GETCHUNKDATA_H

#include "structs.h"
#include <stdio.h>

struct imageHeader get_image_header(FILE *image);
plteArray get_colour_palette(FILE *image, uint32_t nmemb);

#endif // !GETCHUNKDATA_H
