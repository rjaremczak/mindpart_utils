#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PRG_NAME "baseconv"
#define BUF_LEN 10

enum base
{
    BIN = 2,
    DEC = 10,
    HEX = 16,
    INVALID_BASE = 0
};

enum base input_base;
FILE *input_file;

enum base output_base;
FILE *output_file;

char buf[40];
char *bufptr;

static enum base parse_base(char *str)
{
    switch (*str)
    {
    case 'b':
        return BIN;
    case 'd':
        return DEC;
    case 'h':
        return HEX;
    default:
        return INVALID_BASE;
    }
}

char *byte_to_bin(unsigned char byte, char *buf)
{
    for (uint8_t i = 7, mask = 0x80; mask; i--, mask >>= 1)
    {
        buf[i] = byte & mask ? '1' : '0';
    }
    return buf;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("usage: " PRG_NAME "<input base: r|b|d|h> <input file> <output base: b|d|h> <output file>\n");
        return -1;
    }

    input_base = parse_base(argv[1]);
    if (input_base == INVALID_BASE)
    {
        printf(PRG_NAME ": invalid input base: %s", argv[1]);
        return -2;
    }

    output_file = fopen(argv[4], "w+");
    if (!output_file)
    {
        printf(PRG_NAME ": output file %s could not be created\n", argv[4]);
        return -5;
    }

    input_file = fopen(argv[2], "r");
    if (!input_file)
    {
        printf(PRG_NAME ": input file %s could not be opened\n", argv[2]);
        return -3;
    }

    output_base = parse_base(argv[3]);
    if (output_base == INVALID_BASE)
    {
        printf(PRG_NAME ": invalid output base: %s", argv[3]);
        fclose(input_file);
        return -4;
    }

    int line_counter = 0;

    while (fgets(buf, 20, input_file))
    {
        if (strlen(buf) > 0)
        {
            for (int i = 9; i < 13; i++)
                bufptr = (char *)&buf;
            int val = strtol(buf, &bufptr, input_base);
            if (buf == bufptr)
            {
                printf(PRG_NAME ": line parsing error: \"%s\" base: %d\n", buf, input_base);
                return -5;
            }
            switch (output_base)
            {
            case BIN:
                fprintf(output_file, "%s\n", byte_to_bin(val, buf));
                break;
            case HEX:
                fprintf(output_file, "%02X\n", val);
                break;
            default:
                fprintf(output_file, "%d\n", val);
                break;
            }

            line_counter++;
        }
    }

    fclose(input_file);
    fclose(output_file);
    printf(PRG_NAME ": %d lines written.\n", line_counter);
    return 0;
}