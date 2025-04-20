#include <stdio.h>
#include <stdlib.h>


void show_usage(void) {
	printf("Usage: tung [OPTIONS]\n\n"
    		"Tung - Network DoS Testing Tool (for educational use only)\n\n"
    		"Options:\n"
    		"  -a  --attack <type>      Type of DoS attack to perform\n"
    		"                             Supported: udp, syn, slowloris, icmp, httpflood\n\n"
    		"  -t  --target <ip>        Target IP address (e.g. 192.168.1.100)\n"
    		"  -p  --port <port>        Target port (default: 80)\n"
    		"  -d  --duration <sec>     Duration of the attack in seconds (e.g. 10)\n"
		"  -V  --verboose	   Print debug and notes\n"
    		"  -r  --rate <pps>         Packets per second (optional, default: max speed)\n\n"
    		"  -l  --list-attacks       Show available attacks\n"
    		"  -5  --dry-run            Show what would be done, but don’t actually send anything\n\n"
    		"  -h  --help               Show this help message\n"
    		"  -v  --version            Show version information\n\n"
    		"Examples:\n"
    		"  tung --attack udp --target 192.168.1.10 --port 80 --duration 5\n"
    		"  tung --attack slowloris --target 10.0.0.5 --port 8080 --duration 1800\n\n"
    		"Warning:\n"
    		"  This tool is meant for local lab testing only.\n"
    		"  Do NOT use on networks you do not own or have permission to test.\n");
	exit(EXIT_SUCCESS);
}

void show_attacks_list(void) {
	printf("UDP\n"
		"  udp       - Floods target with random UDP packets at high speed\n"
		"    Usage: ./tung -a udp -t IP -p PORT -d SECONDS -r PPS\n\n"
		"  ampl      - UDP amplification attack (exploits vulnerable servers)\n"
		"    Usage: ./tung -a ampl -t IP -p PORT -d SECONDS\n\n"
		"  fraggle   - UDP broadcast attack (Smurf variant)\n"
		"    Usage: ./tung -a fraggle -t IP/MASK -p PORT -d SECONDS\n\n"
		"  ald       - Application-layer DoS (DNS/SSDP floods)\n"
		"    Usage: ./tung -a ald -t IP -p 53 -d SECONDS\n\n"
		"  sockex    - Exhausts target's socket resources\n"
		"    Usage: ./tung -a sockex -t IP -p PORT -d SECONDS\n\n"
		"Notes:\n"
		"- Replace CAPS values with actual parameters\n"
		"- Duration (-d) is in seconds\n"
		"- Rate (-r) is in packets-per-second (PPS)\n");
	exit(EXIT_SUCCESS);
}
