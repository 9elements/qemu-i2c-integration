#include "mctp-common.h"
#include <signal.h>
#include <stdbool.h>
#include <getopt.h>

static volatile sig_atomic_t keep_running = 1;

static void signal_handler(int signum)
{
	(void)signum;
	keep_running = 0;
}

static void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s -e <eid> [-k]\n", prog);
	fprintf(stderr, "  -e, --eid <eid>    EID to bind to\n");
	fprintf(stderr, "  -k, --keep         Keep reading until Ctrl-C (like nc -l -k)\n");
	fprintf(stderr, "  -h, --help         Show this help message\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	struct sockaddr_mctp src_addr;
	socklen_t src_addrlen;
	char recv_buf[BUFFER_SIZE];
	int sd, rc;
	int eid = -1;
	bool keep_reading = false;
	int opt;

	static struct option long_options[] = {
		{"eid",  required_argument, 0, 'e'},
		{"keep", no_argument,       0, 'k'},
		{"help", no_argument,       0, 'h'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "e:kh", long_options, NULL)) != -1) {
		switch (opt) {
		case 'e':
			eid = atoi(optarg);
			break;
		case 'k':
			keep_reading = true;
			break;
		case 'h':
		default:
			usage(argv[0]);
		}
	}

	if (eid < 0) {
		fprintf(stderr, "Error: EID is required\n");
		usage(argv[0]);
	}

	/* Setup signal handler for Ctrl-C */
	if (keep_reading) {
		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);
	}

	/* Create and bind socket */
	sd = mctp_socket_bind(eid);

	do {
		src_addrlen = sizeof(src_addr);
		rc = recvfrom(sd, recv_buf, sizeof(recv_buf) - 1, 0,
		              (struct sockaddr *)&src_addr, &src_addrlen);

		if (rc < 0) {
			if (!keep_running)
				break;
			err(EXIT_FAILURE, "recvfrom() failed");
		}

		/* Null-terminate and print to stdout */
		recv_buf[rc] = '\0';
		printf("%s", recv_buf);
		fflush(stdout);

		if (!keep_reading)
			break;

	} while (keep_running);

	close(sd);
	return EXIT_SUCCESS;
}
