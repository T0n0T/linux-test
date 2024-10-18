#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

volatile int sent_bytes = 0;
volatile int received_bytes = 0;
int echo_mode = 0; // 新增：控制回环模式的变量

speed_t get_baud(int baud)
{
    switch (baud) {
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    default:
        return B9600;
    }
}

int setup_serial(char* device, int baud)
{
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("Opening serial port failed");
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    speed_t baud_rate = get_baud(baud);
    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

void* send_thread(void* arg)
{
    int fd = *(int*)arg;
    int count = 0;
    char message[100];
    while (1) {
        snprintf(message, sizeof(message), "Hello, serial! %d\r\n", count);
        int n = write(fd, message, strlen(message));
        if (n > 0) {
            sent_bytes += n;
            printf("\rSent bytes: %d\t Received bytes: %d", sent_bytes, received_bytes);
            fflush(stdout);
        }
        count++;
        sleep(1);
    }
    return NULL;
}

void* receive_thread(void* arg)
{
    int fd = *(int*)arg;
    char buf[100];
    while (1) {
        int n = read(fd, buf, sizeof buf);
        if (n > 0) {
            received_bytes += n;
            printf("\rSent bytes: %d\t Received bytes: %d", sent_bytes, received_bytes);
            fflush(stdout);
            if (echo_mode) {
                write(fd, buf, n); // 回环：接收到的数据重新发送
            }
        }
    }
    return NULL;
}

int main(int argc, char** argv)
{
    int opt;
    char* serial_port = "/dev/ttyUSB0";
    int baud_rate = 9600;

    while ((opt = getopt(argc, argv, "p:b:e")) != -1) {
        switch (opt) {
        case 'p':
            serial_port = optarg;
            break;
        case 'b':
            baud_rate = atoi(optarg);
            break;
        case 'e':
            echo_mode = 1; // 通过 -e 参数开启回环模式
            break;
        default:
            fprintf(stderr, "Usage: %s -p serial_port -b baud_rate [-e]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    int fd = setup_serial(serial_port, baud_rate);
    if (fd < 0)
        return 1;

    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, send_thread, &fd);
    pthread_create(&tid2, NULL, receive_thread, &fd);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    close(fd);
    return 0;
}