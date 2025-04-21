#include <stdio.h>
#include <stdlib.h>

#define PRG_NAME "parsefon"

FILE* input_file;
void* file_buf = NULL;

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
    file_buf = malloc(fsize + 1);
    long rsize = fread(file_buf, 1, fsize, input_file);
    if(rsize != fsize) {
        printf("only %ld B read\n", rsize);
        goto end_program;
    }

end_program:

    fclose(input_file);
    if(file_buf) free(file_buf);

    return 0;
}