#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "crc16.h"

int set_interface_attribs(int fd, int speed) {
    struct termios tty;

    if(tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

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

    if(tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount) {
    struct termios tty;

    if(tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5; /* half second timer */

    if(tcsetattr(fd, TCSANOW, &tty) < 0) printf("Error tcsetattr: %s\n", strerror(errno));
}

static void file_error(const char* fname) {
    printf("file %s error %s\n", fname, strerror(errno));
}

static void close_fd(int fd) {
    if(fd != -1) {
        tcdrain(fd);
        close(fd);
    }
}

int main(int argc, char* argv[]) {
    const char* src_fname = argv[1];
    const char* dst_fname = argv[2];
    int exit_code = -1;
    int src_fd = -1;
    int dst_fd = -1;

    src_fd = open(src_fname, O_RDONLY);
    if(src_fd == -1) {
        file_error(src_fname);
        goto exit;
    }

    dst_fd = open(dst_fname, O_RDWR | O_NOCTTY | O_SYNC);
    if(dst_fd == -1) {
        file_error(dst_fname);
        goto exit;
    }

    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(dst_fd, B115200);
    //set_mincount(fd, 0);                /* set to pure timed read */

    struct stat src_stat;
    stat(src_fname, &src_stat);

    char buf;
    struct crc16_calc crc16;
    crc16_init(&crc16);

    for(long i = 0; i < src_stat.st_size; i++) {
        if(read(src_fd, &buf, 1) != 1) {
            file_error(src_fname);
            goto exit;
        }
        if(write(dst_fd, &buf, 1) != 1) {
            file_error(dst_fname);
            goto exit;
        }
        crc16_byte(&crc16, buf);
        usleep(5000);
    }
    printf("success, %ldB sent, crc16=0x%04X.\n", src_stat.st_size, crc16.crc);
    exit_code = 0;

exit:
    close_fd(src_fd);
    close_fd(dst_fd);
    return exit_code;
}