#include <stdlib.h>
#include <zlib.h>
#include <stdio.h>
#include <assert.h>

#define CHUNK 16384

int decompress(unsigned char *image_data, int idata_size, unsigned char **ptr_to_uncompressed_image_data, int *uncompressed_data_total_size, int *uncompressed_data_used_data) {
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char *uncompressed_image_data_cursor = *ptr_to_uncompressed_image_data;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = 0;
    strm.avail_out = *uncompressed_data_total_size;
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
        fprintf(stderr, "Initialising strm failed with code %i", ret);
        return 7;
    }

    do {
        strm.avail_in = idata_size;
        strm.next_in = image_data;
        
        do {
            strm.avail_out = CHUNK;
            strm.next_out = uncompressed_image_data_cursor;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);
            if (ret == Z_MEM_ERROR) {
                (void) inflateEnd(&strm);
                fprintf(stderr, "Inflate memory error, stopping\n");
                return 7;
            }

            have = CHUNK - strm.avail_out;
            *uncompressed_data_used_data += have;
            if (have == CHUNK) {
                *ptr_to_uncompressed_image_data = realloc(*ptr_to_uncompressed_image_data, *uncompressed_data_total_size + CHUNK);
                uncompressed_image_data_cursor = *ptr_to_uncompressed_image_data + *uncompressed_data_total_size;
                *uncompressed_data_total_size += CHUNK;
            }

        } while (strm.avail_out == 0);

    } while (ret != Z_STREAM_END);

    (void) inflateEnd(&strm);

    /*for (int i = 0; i < *uncompressed_data_used_data; i++) {
        printf("%02X ", (*ptr_to_uncompressed_image_data)[i]);
        if ((i + 1) % 8 == 0)
            putchar(10);
    }*/

    return 0;
}
