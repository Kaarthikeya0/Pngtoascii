#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "hdr/structs.h"

int constructimage(image *imagedata, point imagesize) {
    imagedata->imagedimensions = imagesize;

    imagedata->imagedata = (triple **) malloc(imagedata->imagedimensions.y * sizeof(triple *));
    if (imagedata->imagedata == NULL) {
        perror("malloc failed");
        return 5;
    }

    for (int i = 0; i < imagedata->imagedimensions.y; i++) {
        imagedata->imagedata[i] = malloc(imagedata->imagedimensions.x * sizeof(triple));
        if (imagedata->imagedata[i] == NULL) {
            perror("malloc failed");
            return 5;
        }
    }
    return 0;
}

int constructgreyimage(greyimage *imagedata, point imagesize) {
    imagedata->imagedimensions = imagesize;

    imagedata->imagedata = (uint8_t **) malloc(imagedata->imagedimensions.y * sizeof(uint8_t *));
    if (imagedata->imagedata == NULL) {
        perror("malloc failed");
        return 5;
    }

    for (int i = 0; i < imagedata->imagedimensions.y; i++) {
        imagedata->imagedata[i] = malloc(imagedata->imagedimensions.x * sizeof(uint8_t));
        if (imagedata->imagedata[i] == NULL) {
            perror("malloc failed");
            return 5;
        }
    }

    return 0;
}

int freeimage(image *imagedata) {
    for (int i = 0; i < imagedata->imagedimensions.y; i++) {
        free(imagedata->imagedata[i]);
    }

    free(imagedata->imagedata);
    return 0;
}

int freegreyimage(greyimage *imagedata) {
    for (int i = 0; i < imagedata->imagedimensions.y; i++) {
        free(imagedata->imagedata[i]);
    }

    free(imagedata->imagedata);
    return 0;
}
