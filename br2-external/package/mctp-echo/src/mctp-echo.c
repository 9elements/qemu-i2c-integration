#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <linux/mctp.h>

#define SENDER_EID 8
#define RECEIVER_EID 9
#define MCTP_TYPE 5

int main(void)
{
	struct sockaddr_mctp addr = { 0 };
	struct sockaddr_mctp src_addr;
	socklen_t src_addrlen;
	char send_buf[] = "Hello from EID 8 via I2C!";
	char recv_buf[4096];
	int sd, rc;
	pid_t pid;

	printf("MCTP I2C Echo Test Application\n");
	printf("Sender EID: %d (mctpi2c1), Receiver EID: %d (mctpi2c2)\n",
	       SENDER_EID, RECEIVER_EID);

	/* Fork to create separate sender and receiver processes */
	pid = fork();
	if (pid < 0) {
		err(EXIT_FAILURE, "fork() failed");
	} else if (pid == 0) {
		/* Child process - RECEIVER on EID 9 */
		sleep(1); /* Brief delay to ensure receiver starts first */

		sd = socket(AF_MCTP, SOCK_DGRAM, 0);
		if (sd < 0)
			err(EXIT_FAILURE, "receiver: socket() failed");

		/* Bind to EID 9 */
		addr.smctp_family = AF_MCTP;
		addr.smctp_network = MCTP_NET_ANY;
		addr.smctp_addr.s_addr = RECEIVER_EID;
		addr.smctp_type = MCTP_TYPE;
		addr.smctp_tag = MCTP_TAG_OWNER;

		rc = bind(sd, (struct sockaddr *)&addr, sizeof(addr));
		if (rc < 0)
			err(EXIT_FAILURE, "receiver: bind() to EID %d failed", RECEIVER_EID);

		printf("[RECEIVER] Bound to EID %d, waiting for messages...\n", RECEIVER_EID);

		src_addrlen = sizeof(src_addr);
		rc = recvfrom(sd, recv_buf, sizeof(recv_buf), 0,
		              (struct sockaddr *)&src_addr, &src_addrlen);

		if (rc < 0)
			err(EXIT_FAILURE, "receiver: recvfrom() failed");

		printf("[RECEIVER] Got message from EID %d (%d bytes): '%s'\n",
		       src_addr.smctp_addr.s_addr, rc, recv_buf);
		printf("[RECEIVER] ✓ Echo test successful!\n");

		close(sd);
		exit(EXIT_SUCCESS);
	} else {
		/* Parent process - SENDER on EID 8 */
		sleep(2); /* Give receiver time to bind and listen */

		sd = socket(AF_MCTP, SOCK_DGRAM, 0);
		if (sd < 0)
			err(EXIT_FAILURE, "sender: socket() failed");

		/* Bind to EID 8 */
		addr.smctp_family = AF_MCTP;
		addr.smctp_network = MCTP_NET_ANY;
		addr.smctp_addr.s_addr = SENDER_EID;
		addr.smctp_type = MCTP_TYPE;
		addr.smctp_tag = MCTP_TAG_OWNER;

		rc = bind(sd, (struct sockaddr *)&addr, sizeof(addr));
		if (rc < 0)
			err(EXIT_FAILURE, "sender: bind() to EID %d failed", SENDER_EID);

		printf("[SENDER] Bound to EID %d\n", SENDER_EID);

		/* Prepare destination address */
		memset(&addr, 0, sizeof(addr));
		addr.smctp_family = AF_MCTP;
		addr.smctp_network = MCTP_NET_ANY;
		addr.smctp_addr.s_addr = RECEIVER_EID;
		addr.smctp_type = MCTP_TYPE;
		addr.smctp_tag = MCTP_TAG_OWNER;

		printf("[SENDER] Sending message to EID %d via mctpi2c1 (I2C addr 0x20): '%s'\n",
		       RECEIVER_EID, send_buf);

		rc = sendto(sd, send_buf, sizeof(send_buf), 0,
		            (struct sockaddr *)&addr, sizeof(addr));

		if (rc != sizeof(send_buf))
			err(EXIT_FAILURE, "sender: sendto() failed");

		printf("[SENDER] Message sent successfully (%d bytes)\n", rc);

		close(sd);
		wait(NULL); /* Wait for receiver child to finish */
		printf("\nMCTP I2C Echo test completed.\n");
	}

	return EXIT_SUCCESS;
}
