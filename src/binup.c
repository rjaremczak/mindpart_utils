#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "crc16.h"

struct bin_header {
    uint8_t type_id;
    uint16_t data_size;
    uint16_t load_address;
} __attribute__((packed));

enum error_code {
    NO_ERROR = 0,
    OPEN_SRC_ERROR = -1,
    OPEN_DST_ERROR = -2,
    GET_ATTRIBS_ERROR = -3,
    SET_ATTRIBS_ERROR = -4,
    READ_ERROR = -10,
    WRITE_ERROR = -11
};

static void close_fd(int fd) {
    if(fd != -1) {
        tcdrain(fd);
        close(fd);
    }
}

static enum error_code set_attribs_fd(int fd, int speed) {
    struct termios tty;

    if(tcgetattr(fd, &tty) != 0) return errno == ENOTTY ? NO_ERROR : GET_ATTRIBS_ERROR;

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD); /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      /* 8-bit characters */
    tty.c_cflag &= ~PARENB;  /* no parity bit */
    tty.c_cflag &= ~CSTOPB;  /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if(tcsetattr(fd, TCSANOW, &tty) != 0) return errno == ENOTTY ? NO_ERROR : GET_ATTRIBS_ERROR;

    return NO_ERROR;
}

static enum error_code transfer_fd(int src_fd, int dst_fd, size_t len, struct crc16_proc* crc16) {
    char data;
    for(size_t i = 0; i < len; i++) {
        if(read(src_fd, &data, 1) != 1) return READ_ERROR;
        if(write(dst_fd, &data, 1) != 1) return WRITE_ERROR;
        if(crc16) crc16_byte(crc16, data);
        usleep(5000);
    }
    printf("transferred: %ld\n", len);
    return NO_ERROR;
}

static enum error_code send_fd(const void* data, size_t len, int dst_fd, struct crc16_proc* crc16) {
    for(size_t i = 0; i < len; i++) {
        if(write(dst_fd, ((const char*)data) + i, 1) != 1) return -2;
        if(crc16) crc16_byte(crc16, ((const char*)data)[i]);
        usleep(5000);
    }
    printf("sent: %ld\n", len);
    return NO_ERROR;
}

int main(int argc, char* argv[]) {
    const char* src_fname = argv[1];
    const char* dst_fname = argv[2];
    enum error_code error_code;
    int src_fd = -1;
    int dst_fd = -1;

    if((src_fd = open(src_fname, O_RDONLY)) == -1) {
        error_code = OPEN_DST_ERROR;
        goto error_exit;
    }

    if((dst_fd = open(dst_fname, O_RDWR | O_NOCTTY | O_SYNC | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
        error_code = OPEN_DST_ERROR;
        goto error_exit;
    }

    struct stat src_stat;
    stat(src_fname, &src_stat);

    if((error_code = set_attribs_fd(dst_fd, B115200)) != NO_ERROR) goto error_exit;

    struct crc16_proc crc16;
    crc16_init(&crc16);
    struct bin_header header = {.type_id = 0x01, .data_size = src_stat.st_size, .load_address = 0xE000};

    // if((error_code = send_fd(&header, sizeof(header), dst_fd, &crc16))) goto error_exit;
    uint16_t written = 0;
    uint16_t size = src_stat.st_size;
    if((error_code = send_fd(&size, sizeof(size), dst_fd, &crc16))) goto error_exit;
    written += sizeof(size);
    if((error_code = transfer_fd(src_fd, dst_fd, src_stat.st_size, &crc16)) != 0) goto error_exit;
    written += src_stat.st_size;
    // if((error_code = send_fd(&crc16, sizeof(crc16.crc), dst_fd, NULL))) goto error_exit;
    error_code = NO_ERROR;

error_exit:
    close_fd(src_fd);
    close_fd(dst_fd);

    switch(error_code) {
        case NO_ERROR:
            printf("success, %u B written, crc16=0x%04x\n", written, crc16.crc);
            break;
        case OPEN_SRC_ERROR:
            printf("file %s opening error: %s\n", src_fname, strerror(errno));
            break;
        case OPEN_DST_ERROR:
            printf("file %s opening error: %s\n", dst_fname, strerror(errno));
            break;
        case GET_ATTRIBS_ERROR:
            printf("tcgetattr error: %s(%i)\n", strerror(errno), errno);
            break;
        case SET_ATTRIBS_ERROR:
            printf("tcsetattr error: %s\n", strerror(errno));
            break;
        case READ_ERROR:
            printf("read error: %s\n", strerror(errno));
            break;
        case WRITE_ERROR:
            printf("write error: %s\n", strerror(errno));
            break;
    }
    return error_code;
}