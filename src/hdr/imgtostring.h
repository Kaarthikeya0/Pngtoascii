#ifndef IMGTOSTRING_H
#define IMGTOSTRING_H

#include "structs.h"
#include <stdbool.h>

int pngtostring(char *pngfile, struct coloured_string *dest, int target_rowlen, bool blur, bool swap_chars, bool square);

#endif // !IMGTOSTRING_H

