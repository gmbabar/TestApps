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
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
	{
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sa.sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}

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
int setup_recvmsg_request(int sfd, struct request *req, unsigned flags)
{
	fprintf(stdout, "%s\n", __func__);
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_recvmsg(sqe, sfd, &(req->msgs), flags);
	io_uring_sqe_set_data(sqe, (void *)req);
	io_uring_submit(&ring);
	return 0;
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
int get_line(const char *src, char *dest, int dest_sz) {
	fprintf(stdout, "%s, size: %d, %p, %p\n", __func__, dest_sz, src, dest);
    for (int i = 0; i < dest_sz; i++) {
        dest[i] = src[i];
		fprintf(stdout, "%c \n", src[i]);
		if (src[i] == 0 ) return 0;
        if (src[i] == '\r' && src[i+1] == '\n') {
            dest[i] = '\0';
            return 0;
        }
    }
    return 0;		// TODO: Proper error handling
}

// ------------------------------------------------
int main(int argc, char *argv[])
{
	struct request sreq;
	int port = 1234;
	int sockfd = setup_socket(port);
	setup_context();
	setup_request(&sreq);
	setup_recvmsg_request(sockfd, &sreq, 0);

	// do the job here:
	struct io_uring_cqe *cqe;
	fprintf(stdout, "%s: Waiting for data\n", __func__);
	int ret = io_uring_wait_cqe(&ring, &cqe);
	struct request *req = (struct request *)cqe->user_data;
	if (ret < 0 || cqe->res < 0)
	{
		if (cqe->res < 0) perror("io_uring_wait_cqe");
		else fprintf(stderr, "Async request failed: %s for event: %d\n",
				strerror(cqe->res), req->event_type);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Received: %d bytes\n", cqe->res);

	// Set string terminator 
	char *data = req->iovecs.iov_base;
	data[cqe->res] = 0;
	fprintf(stdout, "Data: %s\n", data);

	fprintf(stdout, "%s: closing iouring queue\n", __func__);
	io_uring_queue_exit(&ring);
	return ret;
}
