#include <stdio.h>
#include <stdlib.h>


void show_usage(char **argv) {
	printf("Usage: tung [OPTIONS]\n\n"
    		"Tung - Network DoS Testing Tool (for educational use only)\n\n"
    		"Options:\n"
    		"  -a  --attack <type>      Type of DoS attack to perform\n"
    		"                             Supported: udp, syn, slowloris, icmp, httpflood\n\n"
    		"  -t  --target <ip>        Target IP address (e.g. 192.168.1.100)\n"
    		"  -p  --port <port>        Target port (default: 80)\n"
    		"  -d  --duration <sec>     Duration of the attack in seconds (e.g. 10)\n"
		"  -V  --verbose            Print debug and notes\n"
    		"  -r  --rate <pps>         Packets per second (optional, default: max speed)\n\n"
    		"  -l  --list-attacks       Show available attacks\n"
    		"  -5  --dry-run            Show what would be done, but don’t actually send anything\n\n"
		"  -8  --scan <range>       Port scanner mode\n"
		"                             Example: %s --scan 30-40,60-70 --target 34.54.65.34\n"
		"                             Example: %s --scan all --target 34.54.65.34\n"
		"                             Example: %s --scan common --target 34.54.65.34\n\n"
    		"  -h  --help               Show this help message\n"
    		"  -v  --version            Show version information\n\n"
    		"Examples:\n"
    		"  %s --attack udp --target 192.168.1.10 --port 80 --duration 5\n"
    		"  %s --attack slowloris --target 10.0.0.5 --port 8080 --duration 1800\n\n"
    		"Warning:\n"
    		"  This tool is meant for local lab testing only.\n"
    		"  Do NOT use on networks you do not own or have permission to test.\n", 
		argv[0], argv[0], argv[0], argv[0], argv[0]);
	exit(EXIT_SUCCESS);
}

void show_attacks_list(char **argv) {
	const char *bin = argv[0];
	printf("TUNG ATTACKS LIST\n"
		"-----------------\n"
		"UDP\n"
		"  udp       - Floods target with random UDP packets at high speed\n"
		"  ampl      - UDP amplification attack (exploits vulnerable servers)\n"
		"  fraggle   - UDP broadcast attack (Smurf variant)\n"
		"  ald       - Application-layer DoS (DNS/SSDP floods)\n"
		"  sockex    - Exhausts target's socket resources\n"
		"TCP\n"
        	"  syn       - SYN flood attack (half-open connections)\n"
        	"  ack       - ACK flood attack (consumes firewall resources)\n"
        	"  rst       - RST packet flood attack (disrupts active connections)\n"
        	"  fin       - FIN packet flood attack\n"
        	"  psh_ack   - PSH+ACK packet flood attack\n"
        	"  xmas      - XMAS packet flood attack (FIN+PSH+URG flags)\n"
        	"  null      - NULL packet flood attack (no flags set)\n\n"
       		"Example:\n"
		"  Usage: %s -a udp -t IP -p PORT -d SECONDS -r PPS\n\n"
		"Notes:\n"
		"- Replace CAPS values with actual parameters\n"
		"- Duration (-d) is in seconds\n"
		"- Rate (-r) is in packets-per-second (PPS)\n",
		bin
	);	
	exit(EXIT_SUCCESS);
}
