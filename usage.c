#include <stdio.h>
#include <stdlib.h>


void show_usage(void) {
	printf("Usage: tung [OPTIONS]\n\n"
    		"Tung - Network DoS Testing Tool (for educational use only)\n\n"
    		"Options:\n"
    		"  --attack <type>      Type of DoS attack to perform\n"
    		"                       Supported: udp, syn, slowloris, icmp, httpflood\n\n"
    		"  --target <ip>        Target IP address (e.g. 192.168.1.100)\n"
    		"  --port <port>        Target port (default: 80)\n"
    		"  --duration <sec>     Duration of the attack in seconds (e.g. 10)\n"
    		"  --rate <pps>         Packets per second (optional, default: max speed)\n\n"
    		"  --list-attacks       Show available attacks\n"
    		"  --dry-run            Show what would be done, but don’t actually send anything\n\n"
    		"  --help               Show this help message\n"
    		"  --version            Show version information\n\n"
    		"Examples:\n"
    		"  tung --attack udp --target 192.168.1.10 --port 80 --duration 5\n"
    		"  tung --attack slowloris --target 10.0.0.5 --port 8080 --duration \n\n"
    		"Warning:\n"
    		"  This tool is meant for local lab testing only.\n"
    		"  Do NOT use on networks you do not own or have permission to test.\n");
	exit(0);
}
