#ifndef IMGTOSTRING_H
#define IMGTOSTRING_H

#include <stdbool.h>

int pngtostring(char *pngfile, char **dest, int target_rowlen, bool blur);

#endif // !IMGTOSTRING_H

