#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "fnt_header.h"

#define PRG_NAME "parsefon"

u16 word_le(const u8* buf) {
    return buf[0] + (buf[1] << 8);
}

u16 dword_le(const u8* buf) {
    return word_le(buf) + (word_le(buf + 2) << 16);
}

size_t find_ne_header(const u8* buf, size_t size) {
    if(size < 0x3e) return 0;
    size_t sig_ind = word_le(&buf[0x3c]);
    if(buf[sig_ind] != 'N' || buf[sig_ind + 1] != 'E') return 0;
    return sig_ind;
}

void parse_fnt(const u8* data, size_t size, FILE* outfile) {
    const struct fnt_header* fnt = (const struct fnt_header*)data;
    if(size < sizeof(struct fnt_header)) {
        printf("fnt data size: %ld - too short", size);
        return;
    }

    printf("version    : %04x\n", fnt->version);
    printf("type       : %04x\n", fnt->type);
    if(fnt->type & 1) {
        puts("vector fonts not supported");
        return;
    }
    if(!fnt->facename_off || fnt->facename_off > size) {
        puts("font-face not provided");
        return;
    }
    printf("font-face  : %s\n", data + fnt->facename_off);
    printf("copyright  : %.60s\n", fnt->copyright);
    printf("font size  : %dx%d\n", fnt->pix_width, fnt->pix_height);
    printf("char codes : %02x-%02x\n", fnt->first_char, fnt->last_char);

    size_t ctstart_ind = 0x94;
    size_t ctsize = 6;
    if(fnt->version == 0x200) {
        ctstart_ind = 0x76;
        ctsize = 4;
    }

    for(int code = fnt->first_char; code <= fnt->last_char; code++) {
        const u8* entry = data + ctstart_ind + (ctsize * (code - fnt->first_char));
        size_t width = word_le(entry);
        size_t off = ctsize == 4 ? word_le(entry + 2) : dword_le(entry + 2);
        size_t widthbytes = (width + 7) >> 3;
        printf("-- %02x --\n", code);
        for(int line = 0; line < fnt->pix_height; line++) {
            for(int b = 0; b < widthbytes; b++) {
                u8 byte = data[off + b * fnt->pix_height + line];
                fprintf(outfile, "%02X\n", byte);
                for(u8 mask = 0x80; mask; mask >>= 1) { putchar(byte & mask ? '*' : '.'); }
                putchar('\n');
            }
        }
    }
}

void* parse_ne_header(const u8* buf, size_t size, size_t off, FILE* outfile) {
    void* font = NULL;
    size_t data_off = off + word_le(&buf[off + 0x24]);
    const u8* ptr = buf + data_off;
    size_t granularity = word_le(ptr);
    printf("header %05ld-%05ld : '%c%c', granularity:%ld B\n", data_off, size, buf[off], buf[off + 1], granularity);
    ptr += 2;
    while(1) {
        u16 rtype = word_le(ptr);
        if(!rtype) {
            printf("end of resource table\n");
            break;
        }
        u16 count = word_le(ptr + 2);
        ptr += 8;
        for(int i = 0; i < count; i++) {
            u16 rt_start = word_le(ptr) << granularity;
            u16 rt_size = word_le(ptr + 2) << granularity;
            printf("block %05d-%05d : type=%04x\n", rt_start, rt_start + rt_size, rtype);
            if(rt_start == 0 | rt_size == 0 | rt_start + rt_size > size) {
                puts("resource entry size too big");
                goto err;
            }
            if(rtype == 0x8008) {
                parse_fnt(buf + rt_start, rt_size, outfile);
            } else {
                puts("ignore");
            }
            ptr += 12;
        }
    }
err:
    return font;
}

int main(int argc, char* argv[]) {
    FILE* input_file = NULL;
    FILE* output_file = NULL;
    u8* fbuf = NULL;

    if(argc != 3) {
        printf("usage: " PRG_NAME "<input FON file> <output file prefix>\n");
        return -1;
    }

    input_file = fopen(argv[1], "rb");
    if(!input_file) {
        printf("input file %s could not be opened\n", argv[1]);
        goto end_program;
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

    output_file = fopen(argv[2], "wb");
    if(!output_file) {
        printf("output file %s could not be opened\n", argv[2]);
        goto end_program;
    }
    parse_ne_header(fbuf, fsize, ne_off, output_file);

end_program:

    if(input_file) fclose(input_file);
    if(output_file) fclose(output_file);
    free(fbuf);
    return 0;
}