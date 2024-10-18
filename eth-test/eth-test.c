#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
        exit(1);
    }
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);
    struct ethtool_cmd edata;
    edata.cmd = ETHTOOL_GSET;
    ifr.ifr_data = (char*)&edata;
    if (ioctl(sockfd, SIOCETHTOOL, &ifr) == -1) {
        perror("ioctl");
        exit(1);
    }
    printf("Speed: %d Mbps\n", edata.speed);
    close(sockfd);
    return 0;
}
