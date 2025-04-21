#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include "logs.h"
#include "tung.h"

#define TIMEOUT_SEC 1

/* prototypes */
int is_target_up(char *ip_addr, int port);
int resolve_hostname(const char *hostname, char *ip_addr, size_t addr_len);

/* TCP Ping (Returns -1: Down/Error, 0: Up)  */
int is_target_up(char *ip_addr, int port) {
	if(port < 1 || port > 65535)
		return -1;

    	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) {
        	perror("socket");
        	return -1;
    	}

    	struct sockaddr_in addr;
    	memset(&addr, 0, sizeof(addr));
    	addr.sin_family = AF_INET;
    	addr.sin_port = htons(port);
    	addr.sin_addr.s_addr = inet_addr(ip_addr);

    	struct timeval tv;
    	tv.tv_sec = TIMEOUT_SEC;
    	tv.tv_usec = 0;
    	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        	close(sockfd);
        	return -1;     
	}

    	close(sockfd);
    	return 0;  
}

int resolve_hostname(const char *hostname, char *ip_addr, size_t addr_len) {
    	
	if (!hostname || !ip_addr || addr_len < INET_ADDRSTRLEN)
        	return -1;    

    	struct addrinfo hints, *res;
    	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
    	int err = getaddrinfo(hostname, NULL, &hints, &res);
    	
	if (err != 0) {
        	LOG_ERROR("DNS Error: %s\n", gai_strerror(err));
        	return -1;
    	}

    	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    	if (!inet_ntop(AF_INET, &addr->sin_addr, ip_addr, addr_len)) {
        	perror("inet_ntop");
        	freeaddrinfo(res);
        	return -1;
    	}

    	freeaddrinfo(res);
    	return 0; 
}
