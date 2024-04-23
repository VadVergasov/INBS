#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <iostream>

struct pseudo_header {
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;

    struct tcphdr tcp;
};

unsigned short csum(unsigned short* ptr, int nbytes) {
    long sum;
    unsigned short oddbyte;
    short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char*)&oddbyte) = *(u_char*)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return (answer);
}

int main(void) {
    int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if (s == -1) {
        perror("Failed to create socket");
        exit(1);
    }
    unsigned int port = 16384;
    while (1) {
        //Datagram to represent the packet
        char datagram[4096], source_ip[32];
        //IP header
        struct ip* iph = (struct ip*)datagram;
        //TCP header
        struct tcphdr* tcph = (struct tcphdr*)(datagram + sizeof(struct ip));
        struct sockaddr_in sin;
        struct pseudo_header psh;

        strcpy(source_ip, "127.0.0.1");

        sin.sin_family = AF_INET;
        sin.sin_port = htons(12345);
        sin.sin_addr.s_addr = inet_addr("127.0.0.1");

        memset(datagram, 0, 4096);

        //Fill in the IP Header
        iph->ip_hl = 5;
        iph->ip_v = 4;
        iph->ip_tos = 0;
        iph->ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
        iph->ip_id = htons(rand());
        iph->ip_off = 0;
        iph->ip_ttl = 255;
        iph->ip_p = IPPROTO_TCP;
        iph->ip_sum = 0;
        iph->ip_src.s_addr = inet_addr(source_ip);
        iph->ip_dst.s_addr = sin.sin_addr.s_addr;

        iph->ip_sum = csum((unsigned short*)datagram, iph->ip_len >> 1);

        //TCP Header
        std::cout << port << std::endl;
        tcph->th_sport = htons(port);
        tcph->th_dport = htons(12345);
        tcph->th_seq = 0;
        tcph->th_ack = 0;
        tcph->th_off = 5;
        tcph->th_flags = TH_SYN;
        tcph->th_win = htons(5840);
        tcph->th_sum = 0;
        tcph->th_urp = 0;

        psh.source_address = inet_addr(source_ip);
        psh.dest_address = sin.sin_addr.s_addr;
        psh.placeholder = 0;
        psh.protocol = IPPROTO_TCP;
        psh.tcp_length = htons(20);

        memcpy(&psh.tcp, tcph, sizeof(struct tcphdr));

        tcph->th_sum = csum((unsigned short*)&psh, sizeof(struct pseudo_header));

        int one = 1;
        const int* val = &one;
        if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
            printf("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n", errno, strerror(errno));
            exit(0);
        }

        if (sendto(s,                      /* our socket */
                   datagram,               /* the buffer containing headers and data */
                   iph->ip_len,            /* total length of our datagram */
                   0,                      /* routing flags, normally always 0 */
                   (struct sockaddr*)&sin, /* socket addr, just like in */
                   sizeof(sin)) < 0)       /* a normal send() */
        {
            printf("error\n");
        } else {
            printf("Packet Send \n");
        }
        port++;
        port = (port % 32768) + 16384;
    }

    return 0;
}
