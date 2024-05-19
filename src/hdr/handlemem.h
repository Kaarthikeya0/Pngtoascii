#ifndef MEM_H
#define MEM_H
#include "structs.h"

int constructimage(image *imagedata, point imagesize);
int constructgreyimage(greyimage *imagedata, point imagesize);

int freeimage(image *imagedata);
int freegreyimage(greyimage *imagedata);

#endif // !MEM_H
