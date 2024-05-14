#ifndef DATA_PARSE_H
#define DATA_PARSE_H

int paeth_predictor(int a, int b, int c);
int recon_scanline(unsigned char *curr_scanline, unsigned char *prev_scanline, int scanline_len, int scanline_num, int bits_per_pixel, int bytes_per_pixel);

#endif // !DATA_PARSE_H
