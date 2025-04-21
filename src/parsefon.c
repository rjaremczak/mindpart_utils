#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PRG_NAME "parsefon"

FILE* input_file;
uint8_t* fbuf = NULL;

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("usage: " PRG_NAME "<input FON file> <output file prefix>\n");
        return -1;
    }

    input_file = fopen(argv[1], "rb");
    if(!input_file) {
        printf("input file %s could not be opened\n", argv[1]);
        return -3;
    }

    int err = fseek(input_file, 0L, SEEK_END);
    if(err != 0) {
        printf("error getting file size: %d\n", err);
        goto end_program;
    };

    long fsize = ftell(input_file);
    if(fsize == -1) {
        printf("ftell error\n");
        goto end_program;
    }
    printf("reading %ld B ...\n", fsize);

    fseek(input_file, 0, SEEK_SET);
    fbuf = malloc(fsize + 1);
    long rsize = fread(fbuf, 1, fsize, input_file);
    if(rsize != fsize) {
        printf("only %ld B read\n", rsize);
        goto end_program;
    }

    long ne_ind = fbuf[0x3c] + (fbuf[0x3d] << 8);
    if(fbuf[ne_ind] != 'N' || fbuf[ne_ind + 1] != 'E') {
        printf("no NE header\n");
        goto end_program;
    }

    printf("%c%c\n", fbuf[ne_ind], fbuf[ne_ind + 1]);

end_program:

    fclose(input_file);
    if(fbuf) free(fbuf);

    return 0;
}