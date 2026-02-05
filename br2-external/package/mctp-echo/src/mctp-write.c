#include "mctp-common.h"
#include <getopt.h>

static void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s -e <eid> -d <dest_eid>\n", prog);
	fprintf(stderr, "  -e, --eid <eid>         EID to bind to\n");
	fprintf(stderr, "  -d, --dest <dest_eid>   Destination EID to send to\n");
	fprintf(stderr, "  -h, --help              Show this help message\n");
	fprintf(stderr, "\nReads message from STDIN and sends it via MCTP.\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	struct sockaddr_mctp dest_addr;
	char send_buf[BUFFER_SIZE];
	int sd, rc;
	int eid = -1;
	int dest_eid = -1;
	int opt;
	size_t total_read = 0;
	ssize_t n;

	static struct option long_options[] = {
		{"eid",  required_argument, 0, 'e'},
		{"dest", required_argument, 0, 'd'},
		{"help", no_argument,       0, 'h'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "e:d:h", long_options, NULL)) != -1) {
		switch (opt) {
		case 'e':
			eid = atoi(optarg);
			break;
		case 'd':
			dest_eid = atoi(optarg);
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

	if (dest_eid < 0) {
		fprintf(stderr, "Error: Destination EID is required\n");
		usage(argv[0]);
	}

	/* Read from stdin */
	while (total_read < sizeof(send_buf) - 1) {
		n = read(STDIN_FILENO, send_buf + total_read,
		         sizeof(send_buf) - 1 - total_read);
		if (n <= 0)
			break;
		total_read += n;
	}

	if (total_read == 0) {
		fprintf(stderr, "Error: No data read from STDIN\n");
		exit(EXIT_FAILURE);
	}

	/* Create and bind socket */
	sd = mctp_socket_bind(eid);

	/* Prepare destination address */
	mctp_prepare_dest_addr(&dest_addr, dest_eid);

	/* Send the message */
	rc = sendto(sd, send_buf, total_read, 0,
	            (struct sockaddr *)&dest_addr, sizeof(dest_addr));

	if (rc < 0)
		err(EXIT_FAILURE, "sendto() failed");

	if ((size_t)rc != total_read)
		errx(EXIT_FAILURE, "sendto() sent %d bytes, expected %zu", rc, total_read);

	close(sd);
	return EXIT_SUCCESS;
}
