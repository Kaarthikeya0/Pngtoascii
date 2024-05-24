#ifndef IMGTOMEM_H
#define IMGTOMEM_H

#include "structs.h"
#include <stdio.h>


int image_to_memory(struct imageHeader hdr, plteArray plt, FILE *imagefile, unsigned char **image_data, int *idata_size, unsigned char **uncompressed_image_data);

#endif // !IMGTOMEM_H

