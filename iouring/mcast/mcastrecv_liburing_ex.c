/* mcastrecv_liburing_ex
 *	 Test code to send multiple messages.
 * Option-1: Create multiple requests, submit all to SQE, then submit queue 
 * Option-2: Create multiple iovecs under one call
 * 
 * gcc  -o mcastrecv_liburing_ex mcastrecv_liburing.c -luring
 */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "liburing.h"

// ------------------------------------------------
#define QUE_DEPTH 16		 // Queue Depth

#define EVENT_TYPE_RECVMSG 0
#define EVENT_TYPE_SENDMSG 1	// consider enum

struct io_uring ring;

// ------------------------------------------------
struct sockaddr_in sa;
struct ip_mreq group;

char interface_ip[] = "127.0.0.1";
char multicast_ip[] = "239.0.0.1";
unsigned short multicast_port = 12345;

#define BUFSIZE 512

struct request
{
	int event_type;
	struct msghdr msgs;
	struct iovec iovecs;
	char buff[BUFSIZE + 1];		// consider refactoring to pointer
};

// ------------------------------------------------
int setup_socket(int port)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
	{
		perror("socket()");
		exit(EXIT_FAILURE);
	}

    /* Enable SO_REUSEADDR to allow multiple instances to run */
    {
        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
        {
            perror("Setting SO_REUSEADDR error");
            close(sockfd);
            exit(1);
        }
        printf("Setting SO_REUSEADDR...OK.\n");
    }

    /* Bind to the proper port number with the IP address */
    memset((char *)&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	//sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}
    printf("Binding datagram socket...OK.\n");

    /* Join the multicast group */
    group.imr_multiaddr.s_addr = inet_addr(multicast_ip);
    group.imr_interface.s_addr = inet_addr(interface_ip);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
    {
        perror("Adding multicast group error");
        close(sockfd);
        exit(1);
    }
    printf("Adding multicast group...OK.\n");


	return (sockfd);
}

// ------------------------------------------------
void setup_context()
{
	fprintf(stdout, "%s\n", __func__);
	int ret = io_uring_queue_init(QUE_DEPTH, &ring, 0);
	if (ret < 0)
	{
		fprintf(stderr, "queue_init: %s\n", strerror(-ret));
		exit(EXIT_FAILURE);
	}
}

// ------------------------------------------------
void setup_recvmsg_request(int sfd, struct request *req, unsigned flags, bool submit)
{
	fprintf(stdout, "%s\n", __func__);
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_recvmsg(sqe, sfd, &(req->msgs), flags);
	io_uring_sqe_set_data(sqe, (void *)req);
	if (submit) io_uring_submit(&ring);		// need Xple elements to load
}

// ------------------------------------------------
void setup_request(struct request *req)
{
	fprintf(stdout, "%s\n", __func__);
	req->event_type = EVENT_TYPE_RECVMSG;
	memset(&req->msgs, 0, sizeof(req->msgs));
	req->iovecs.iov_base = req->buff;
	req->iovecs.iov_len = BUFSIZE;
	req->msgs.msg_iov = &req->iovecs;
	req->msgs.msg_iovlen = 1;
}

// ------------------------------------------------
int main(int argc, char *argv[])
{
	#define QSIZE 10
	struct request sreq[QSIZE];
	int sockfd = setup_socket(multicast_port);
	setup_context();
	for (int idx=0; idx<QSIZE; ++idx) {
		setup_request(&sreq[idx]);
		setup_recvmsg_request(sockfd, &sreq[idx], 0, false);
	}
	io_uring_submit(&ring);	
	// NOTE: call submit_wait() which specifies number of events to occur instead of interrupt on each send/recv

	// do the job here:		// consider putting it under a function
	struct io_uring_cqe *cqe;
	fprintf(stdout, "%s: Waiting for data\n", __func__);
	for (int idx=0; idx<QSIZE*200; ++idx) {
		int ret = io_uring_wait_cqe(&ring, &cqe);
		struct request *req = (struct request *)cqe->user_data;
		/* Mark this completion as seen */
    	io_uring_cqe_seen(&ring, cqe);
		
		if (ret < 0 || cqe->res < 0) {
			if (cqe->res < 0) perror("io_uring_wait_cqe");
			else fprintf(stderr, "Async request failed: %s for event: %d\n",
					strerror(cqe->res), req->event_type);
			exit(EXIT_FAILURE);
		}

		// Set string terminator, not need in my code though.
		char *data = req->iovecs.iov_base;
		data[cqe->res] = 0;
		fprintf(stdout, "Received: %d bytes: Data: %s\n", cqe->res, data);

		// push back entry to queue.
		setup_recvmsg_request(sockfd, req, 0, true);
	}

	fprintf(stdout, "%s: closing iouring queue\n", __func__);
	io_uring_queue_exit(&ring);
	return 0;
}

