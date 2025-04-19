#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>


/* This function check if a given ip addr is 
 * 	- ipv4 (return: 4)
 * 	- ipv6 (return: 6)
 * 	- invalid (return: 0)  */
int validate_ip(const char *ip) {
    	if (ip == NULL || *ip == '\0') {
        	return 0;
    	}

    	const char *p = ip;
    	int has_colon = 0, has_dot = 0;

    	while (*p) {
        	if (*p == ':') has_colon = 1;
        	if (*p == '.') has_dot = 1;
        	if (!isxdigit(*p) && *p != ':' && *p != '.' && *p != '%') {
            		return 0;
        	}
        	p++;
    	}
	
    	/* Ipv4 */
    	if (has_dot && !has_colon) {
        	struct in_addr addr;
        	if (inet_pton(AF_INET, ip, &addr) == 1) {
            		int segments = 0;
            		const char *ptr = ip;

            		while (*ptr && segments < 4) {
                		if (*ptr == '.') {
                    			segments++;
                    			ptr++;
                    			continue;
                		}
                		if (!isdigit(*ptr)) return 0;

                		int num = 0;
                		while (isdigit(*ptr)) {
                   			num = num * 10 + (*ptr - '0');
                    			if (num > 255) return 0;
                   			ptr++;
                		}
                		if (*ptr == '.') continue;
                		if (*ptr == '\0') break;
                		return 0;
            		}
            		return (segments == 3) ? 4 : 0;
        	}
        	return 0;
   	 }

    	/* IPv6 */
    	if (has_colon) {
        	struct in6_addr addr;
        	if (inet_pton(AF_INET6, ip, &addr) == 1) {
            		const char *ptr = ip;
            		int colon_count = 0;
            		int double_colon = 0;
            		int digit_count = 0;
            		int valid_chars = 1;

            		while (*ptr) {
                		if (*ptr == ':') {
                    			colon_count++;
                    			if (*(ptr + 1) == ':') {
                        			if (double_colon) return 0;
                        			double_colon = 1;
                        			ptr++;
                    			}
                    			digit_count = 0;
                		} else if (isxdigit(*ptr)) {
                    			digit_count++;
                    			if (digit_count > 4) return 0;     
     				} else if (*ptr == '%') {
                    			break;   
		  		} else {
                   			valid_chars = 0;
                    			break;
                		}
                		ptr++;
            		}

            		if (!valid_chars) return 0;
            		if (colon_count > 7 && !double_colon) return 0;
            		if (colon_count < 2 && !double_colon) return 0;

            		return 6;
        	}
        	return 0;
    	}

    	return 0;
}
