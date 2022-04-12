// udprecv_liburing
/* 
 * gcc  -o udprecv_liburing udprecv_liburing.c -luring
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

// ------------------------------------------------
#define EVENT_TYPE_RECVMSG 0
#define EVENT_TYPE_SENDMSG 1	// consider enum

struct io_uring ring;

#define BUFSIZE 512

struct request
{
	int event_type;
	struct msghdr msgs;
	struct iovec iovecs;
	char buff[BUFSIZE + 1];		// consider refactoring to pointer
};

// ------------------------------------------------
struct sockaddr_in sa;

// ------------------------------------------------
int setup_socket(int port)
{
	fprintf(stdout, "%s\n", __func__);

	printf("creating socket.");
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
	{
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	printf("connecting socket.");
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sa.sin_port = htons(port);
	if (connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
	{
		perror("connect()");
		exit(EXIT_FAILURE);
	}

	return sockfd;
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
int setup_sendmsg_request(int sfd, struct request *req, unsigned flags)
{
	fprintf(stdout, "%s\n", __func__);
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_sendmsg(sqe, sfd, &(req->msgs), flags);
	io_uring_sqe_set_data(sqe, (void *)req);
	io_uring_submit(&ring);
	return 0;
}

// ------------------------------------------------
void setup_request(struct request *req)
{
	fprintf(stdout, "%s\n", __func__);
	req->event_type = EVENT_TYPE_SENDMSG;
	strcpy(req->buff, "This is io_uring_sendmsg test");
	memset(&req->msgs, 0, sizeof(req->msgs));
	req->iovecs.iov_base = req->buff;
	req->iovecs.iov_len = strlen(req->buff)+1;
	req->msgs.msg_iov = &req->iovecs;
	req->msgs.msg_iovlen = 1;
}

// ------------------------------------------------
int main(int argc, char *argv[])
{
	struct request sreq;
	int port = 1234;
	int sockfd = setup_socket(port);
	setup_context();
	setup_request(&sreq);
	setup_sendmsg_request(sockfd, &sreq, 0);

	// do the job here:
	struct io_uring_cqe *cqe;
	fprintf(stdout, "%s: Waiting sendmsg completion\n", __func__);
	int ret = io_uring_wait_cqe(&ring, &cqe);
	struct request *req = (struct request *)cqe->user_data;
	fprintf(stdout, "%s: Sent data, ret: %d, cqe.res: %d, len: %lu\n", __func__, ret, cqe->res, req->msgs.msg_iovlen);
	if (ret < 0 || cqe->res < 0)
	{
		if (cqe->res < 0) perror("io_uring_wait_cqe");
		else fprintf(stderr, "Async request failed: %s for event: %d\n",
				strerror(cqe->res), req->event_type);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "%s: Data sent: %d bytes\n", __func__, cqe->res);
	fprintf(stdout, "%s: Closing iouring queue\n", __func__);
	io_uring_queue_exit(&ring);

	return ret;
}

