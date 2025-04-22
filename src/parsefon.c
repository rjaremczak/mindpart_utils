#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PRG_NAME "parsefon"

FILE* input_file;
uint8_t* fbuf = NULL;

uint16_t word_le(const uint8_t* buf) {
    return buf[0] + (buf[1] << 8);
}

size_t find_ne_header(const uint8_t* buf, size_t size) {
    if(size < 0x3e) return 0;
    size_t sig_ind = word_le(&buf[0x3c]);
    if(buf[sig_ind] != 'N' || buf[sig_ind + 1] != 'E') return 0;
    return sig_ind;
}

void* parse_ne_header(const uint8_t* buf, size_t size, size_t off) {
    void* font = NULL;
    size_t data_off = off + word_le(&buf[off + 0x24]);
    const uint8_t* ptr = buf + data_off;
    size_t granularity = word_le(ptr);
    printf("header %05ld-%05ld : '%c%c', granularity:%ld B\n", data_off, size, buf[off], buf[off + 1], granularity);
    ptr += 2;
    while(1) {
        uint16_t rtype = word_le(ptr);
        if(!rtype) {
            printf("end of resource table\n");
            break;
        }
        uint16_t count = word_le(ptr + 2);
        ptr += 8;
        for(int i = 0; i < count; i++) {
            uint16_t rt_start = word_le(ptr) << granularity;
            uint16_t rt_size = word_le(ptr + 2) << granularity;
            printf("block %05d-%05d : type=%04x\n", rt_start, rt_start + rt_size, rtype);
            if(rt_start == 0 | rt_size == 0 | rt_start + rt_size > size) {
                puts("resource entry size too big");
                goto err;
            }
            if(rtype == 0x8008) {}
            ptr += 12;
        }
    }
err:
    return font;
}

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

    size_t fsize = ftell(input_file);
    if(fsize == -1) {
        printf("ftell error\n");
        goto end_program;
    }
    printf("reading %ld B ...\n", fsize);

    fseek(input_file, 0, SEEK_SET);
    fbuf = malloc(fsize + 1);
    size_t rsize = fread(fbuf, 1, fsize, input_file);
    if(rsize != fsize) {
        printf("only %ld B read\n", rsize);
        goto end_program;
    }

    size_t ne_off = find_ne_header(fbuf, fsize);
    if(!ne_off) {
        printf("no NE header\n");
        goto end_program;
    }

    parse_ne_header(fbuf, fsize, ne_off);

end_program:

    fclose(input_file);
    if(fbuf) free(fbuf);

    return 0;
}