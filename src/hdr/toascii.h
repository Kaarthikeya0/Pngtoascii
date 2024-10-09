#ifndef TOASCII_H
#define TOASCII_H

#include "structs.h"
#include <stdint.h>


char lumtochar(uint8_t lum);
int blur_image(greyimage *src, greyimage *dest);
int blur_image_rgb(image *src, image *dest);

#endif // !TOASCII_H

