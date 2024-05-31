#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

#include "hdr/imgtostring.h"

int main(int argc, char *argv[]) {

    int target_rowlen = 200;
    bool blur = false;
    bool cons = false;
    char *input = NULL;
    char *output = NULL;

    bool output_file_exists = false;
    bool force_overwrite = false;
    bool user_give_output = false;
    bool swap_chars = false;

    const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"rowlength", required_argument, NULL, 'r'},
        {"blur", no_argument, NULL, 'b'},
        {"console", no_argument, NULL, 'c'},
        {"force", no_argument, NULL, 'f'},
        {"about", no_argument, NULL, 'a'},
        {"swapchars", no_argument, NULL, 's'},
    };

    char opt;
    while ((opt = getopt_long(argc, argv, "hi:o:r:bcfas", long_options, NULL)) != EOF) {
        switch (opt) {
            case 'h':
                printf("Basic usage: .\\asciiart.exe --console --input <path to png>\n"
                       "--input  / -i for input png file (compulsory)\n"
                       "--output / -o for output text file\n"
                       "--force / -f for forcing the program to overwrite an existing file\n"
                       "--rowlength / -r for target row length of each row in the ascii image\n"
                       "--blur / -b to blur the image before converting into an ascii image\n"
                       "--swapchars / -s to toggle black and white representations of the characters\n"
                       "--console / -c to print the ascii image to the console\n"
                       "--about / -a to get a more descriptive explanation of the program\n"
                       );
                exit(0);
            case 'a':
                printf("This program converts a provided png file into an ascii representation of the png\n"
                       "You can use the command line flags to change how the ascii image is generated\n"
                       );
                exit(0);
            case 'i':
                input = optarg;
                if (access(input, F_OK) != 0) {
                    perror("File not found");
                    exit(1);
                }
                break;
            case 'o':
                output = optarg;
                if (access(output, F_OK) == 0) {
                    output_file_exists = true;
                }
                user_give_output = true;
                break;
            case 'r':
                target_rowlen = atoi(optarg);
                break;
            case 'b':
                blur = true;
                break;
            case 'c':
                cons = true;
                break;
            case 'f':
                force_overwrite = true;
                break;
            case 's':
                swap_chars = true;
                break;
            default:
                fprintf(stderr, "Invalid flag given please use the -h flag for help\n");
                exit(1);
        }
    }

    if (output_file_exists && !force_overwrite) {
        fprintf(stderr, "The output file exists and could be overwritten if you continue\nplease use the -f flag if you want it to be overwritten.\n");
        exit(1);
    }

    if (input == NULL) {
        fprintf(stderr, "No input file given, please read the help menu using %s -h\n", argv[0]);
        exit(1);
    }

    // create an output file if no output method defined
    if (!user_give_output && !cons) {
        output = malloc(sizeof(char) * 12);
        if (!force_overwrite) {
            int file_limit = 100;
            for (int i = 0; i < file_limit; i++) {
                sprintf(output, "out%d.txt", i);
                if (access(output, F_OK) != 0) {
                    break;
                }
                if (i == file_limit - 1) {
                    fprintf(stderr, "Hello how do you have %i output files??\n", file_limit);
                    exit(1);
                }
            }
        }
        else {
            sprintf(output, "out%d.txt", 0);
        }
    }

    char *destination = NULL;
    if (pngtostring(input, &destination, target_rowlen, blur, swap_chars)) {
        fprintf(stderr, "You done fucked up\n");
        return 1;
    }
    
    if (cons)
        fprintf(stdout, "%s", destination);

    if (output != NULL) {
        FILE *output_file = fopen(output, "w");
        fprintf(output_file, "%s", destination);
    }

    free(destination);

    if (!user_give_output && !cons)
        free(output);

    return EXIT_SUCCESS;
}
