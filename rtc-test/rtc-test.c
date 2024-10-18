#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <stdint.h>
#include <getopt.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>

static const char *device = "/dev/rtc1";
static const char *settime ="2022-12-1 12:1:00";
static uint32_t readtime = 0;
static int fd =  -1;

static void print_usage(const char *prog)
{
	printf("Usage: %s [-DsbdlHOLC3]\n", prog);
	puts("  -d --device   Device to use (default /dev/rtc1)\n"
	     "  -s --settime  Settime(default 2022-12-1 12:1:00)\n"
         "  -r --readtime Readtime\n" );
	exit(1);
}

static int parse_opts(int argc, char *argv[])
{
    struct rtc_time settime;
    struct rtc_time readtime;
    struct timeval tv;
    struct timezone tz;
    struct tm tm;
    int ret = 0;

	while (1) {
		static const struct option lopts[] = {
			{ "device",required_argument, NULL, 'd' },
			{ "settime",no_argument, NULL, 's' },
            { "readtime",no_argument, NULL, 'r' },
			{ NULL, no_argument, NULL, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "d:sr", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'd':
			device = optarg;
            close(fd);
            if ( ( fd = open( device, O_RDWR ) ) < 0 ) {
                perror("rtc test error in open device\n");
                return 1;
            }
			break;
		case 's':
            settime.tm_year=122;
            settime.tm_mon = 11;
            settime.tm_mday = 1;
            settime.tm_wday = 4;
            settime.tm_hour = 12;
            settime.tm_min = 1;
            settime.tm_sec = 0;
            settime.tm_isdst = 0;
            ret= ioctl(fd, RTC_SET_TIME, &settime);
            if( ret == 0 ){
                ioctl(fd, RTC_RD_TIME, &readtime);
                if ( settime.tm_year != readtime.tm_year || settime.tm_mon != readtime.tm_mon || settime.tm_mday != readtime.tm_mday
                    || settime.tm_hour != readtime.tm_hour || settime.tm_min != readtime.tm_min ) {
                    close(fd);
                    printf("rtc test error in compare time\n ");
                    return -1;
                }
                printf("set time: %d-%d-%d-%d,%d:%d:%d\n" , \
                    settime.tm_year+1900,  settime.tm_mon+1,  settime.tm_mday, settime.tm_wday,  \
                    settime.tm_hour , settime.tm_min, settime.tm_sec);

                tm.tm_sec = settime.tm_sec;
                tm.tm_min = settime.tm_min;
                tm.tm_hour = settime.tm_hour;
                tm.tm_mday = settime.tm_mday;
                tm.tm_mon = settime.tm_mon;
                tm.tm_year = settime.tm_year;
                tm.tm_isdst = -1;
                tv.tv_sec = mktime(&tm); 
                settimeofday(&tv,NULL);

            }else {
                close(fd);
                printf("rtc test error in set time\n");
                return -1;
            }

			break;
        case 'r':      
            ret= ioctl(fd, RTC_RD_TIME, &readtime);
            if( ret == 0 )
            {
                printf("read time: %d-%d-%d-%d,%d:%d:%d \n" , \
                    readtime.tm_year+1900,  readtime.tm_mon+1,  readtime.tm_mday, readtime.tm_wday,  \
                    readtime.tm_hour , readtime.tm_min, readtime.tm_sec);
            }else{
                close(fd);
                printf("rtc test error in read time\n");
                return -1;
            }
            break;
		default:
			print_usage(argv[0]);
			break;
		}
	}

    return 0;
}
int main(int argc, char *argv[])
{
    printf("rtc test begin =====================================================\n");

    if ( fd < 0 ){
        if (( fd = open( device, O_RDWR ) ) < 0) {
                perror("rtc test error in open device\n");
                return 1;
        }
    }

    if ( parse_opts(argc, argv) < 0 ){
        return -1;
    }

    close( fd );
    printf("rtc test end =======================================================\n");
    return 0;
}