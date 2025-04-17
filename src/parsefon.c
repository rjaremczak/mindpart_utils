#include <stdio.h>

#define PRG_NAME "parsefon"

FILE *input_file;

int main(int argc, char *argv[]) {
    if (argc != 3)
    {
        printf("usage: " PRG_NAME "<input FON file> <output file prefix>\n");
        return -1;
    }

    input_file = fopen(argv[1], "r");
    if (!input_file)
    {
        printf(PRG_NAME ": input file %s could not be opened\n", argv[1]);
        return -3;
    }

    return 0;
}