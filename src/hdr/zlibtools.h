#ifndef ZLIBTOOLS_H
#define ZLIBTOOLS_H

int decompress(unsigned char *image_data, int idata_size, unsigned char **ptr_to_uncompressed_image_data, int *uncompressed_data_total_size, int *uncompressed_data_used_data);

#endif // !ZLIBTOOLS_H
