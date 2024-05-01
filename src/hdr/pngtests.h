#ifndef PNGTESTS_H
#define PNGTESTS_H 

#include <stdio.h>
#include <stdbool.h>
#include "structs.h"

bool ispng(FILE *file);
bool has_ihdr(FILE *file);
bool check_bit_depth_vs_colour_type(struct imageHeader hdr);

#endif // !PNGTESTS_H

