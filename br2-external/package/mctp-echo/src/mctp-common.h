#ifndef MCTP_COMMON_H
#define MCTP_COMMON_H

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/mctp.h>

#define MCTP_TYPE 5
#define BUFFER_SIZE 4096

/* Create and bind MCTP socket to given EID */
static inline int mctp_socket_bind(int eid)
{
	struct sockaddr_mctp addr = { 0 };
	int sd, rc;

	sd = socket(AF_MCTP, SOCK_DGRAM, 0);
	if (sd < 0)
		err(EXIT_FAILURE, "socket() failed");

	addr.smctp_family = AF_MCTP;
	addr.smctp_network = MCTP_NET_ANY;
	addr.smctp_addr.s_addr = eid;
	addr.smctp_type = MCTP_TYPE;
	addr.smctp_tag = MCTP_TAG_OWNER;

	rc = bind(sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
		err(EXIT_FAILURE, "bind() to EID %d failed", eid);

	return sd;
}

/* Prepare destination address for sending */
static inline void mctp_prepare_dest_addr(struct sockaddr_mctp *addr, int dest_eid)
{
	memset(addr, 0, sizeof(*addr));
	addr->smctp_family = AF_MCTP;
	addr->smctp_network = MCTP_NET_ANY;
	addr->smctp_addr.s_addr = dest_eid;
	addr->smctp_type = MCTP_TYPE;
	addr->smctp_tag = MCTP_TAG_OWNER;
}

#endif /* MCTP_COMMON_H */
