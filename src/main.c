#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "hdr/imgtostring.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Can you brain\n");
        return 1;
    }

    char *destination = NULL;
    if (pngtostring(argv[1], &destination, 200, true)) {
        fprintf(stderr, "You done fucked up\n");
        return 1;
    }

    printf("%s\n", destination);
    return EXIT_SUCCESS;
}
