#include <fcntl.h>
#include <getopt.h>
#include <linux/input.h>
#include <linux/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static char* device = "/dev/input/event2";
static uint32_t period = 1000;
static uint32_t type = 0;

static void print_usage(const char* prog)
{
    printf("Usage: %s [-d:p:t:]\n", prog);
    puts("  -d --device   device to use (default /dev/input/event2)\n"
         "  -p --period   period (defalut 1000 Hz)\n"
         "  -t --type     type (defalut 0 SND_BELL,1 SND_TONE)\n");
    exit(1);
}

static void parse_opts(int argc, char* argv[])
{
    while (1) {
        static const struct option lopts[] = {
            { "device", required_argument, NULL, 'd' },
            { "period", required_argument, NULL, 'p' },
            { "type", required_argument, NULL, 't' },
            { NULL, no_argument, NULL, 0 },
        };
        int c;

        c = getopt_long(argc, argv, "d:p:t:", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
        case 'd':
            device = optarg;
            break;
        case 'p':
            period = atoi(optarg);
            break;
        case 't':
            type = atoi(optarg);
            break;
        default:
            print_usage(argv[0]);
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    int fd, ret;
    struct input_event event;

    parse_opts(argc, argv);

    if ((fd = open(device, O_RDWR)) < 0) {
        perror("beep test wrong");
        return 1;
    }

    event.type = EV_SND;

    if (type == 0) {
        event.code = SND_BELL;
        for (int i = 0; i <= 2; i++) {
            event.value = period;
            ret = write(fd, &event, sizeof(struct input_event));
            sleep(1);
            event.value = 0;
            ret = write(fd, &event, sizeof(struct input_event));
            sleep(1);
        }
    } else if (type == 1) {
        event.code = SND_TONE;
        for (int i = 0; i <= 2; i++) {
            event.value = period;
            ret = write(fd, &event, sizeof(struct input_event));
            sleep(1);
            event.value = 0;
            ret = write(fd, &event, sizeof(struct input_event));
            sleep(1);
        }
    }

    close(fd);
    return 0;
}