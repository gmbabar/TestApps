//io_uring-cp
/* SPDX-License-Identifier: MIT */
/*
 * gcc -Wall -O2 -D_GNU_SOURCE -o io_uring-cp io_uring-cp.c -luring
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
#include "liburing.h"

#define QUEUE_DEPTH 64		   // Queue Depth
#define BLOCK_SIZE (32 * 1024) // Block Size

// ------------------------------------------------
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Globals - For now
struct in_addr localInterface;
struct sockaddr_in groupSockAddr;
int sockfd;
char databuf[1024] = "Obesity in the United States is a major health issue resulting in numerous diseases,"
					 "specifically increased risk of certain types of cancer, coronary artery disease, type 2 diabetes, stroke, "
					 "as well as significant increases in early mortality and economic costs.";
// int datalen = sizeof(databuf);
int datalen = strlen(databuf) + 1;

char interface_ip[] = "127.0.0.1";
char multicast_ip[] = "239.0.0.1";
unsigned short multicast_port = 12345;
int count = 10;

int setup_socket(int port)
{
	/* Create a datagram socket on which to send. */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	std::cout << "Opening the datagram socket...OK" << std::endl;

	/* Initialize the group sockaddr struct with group addr */
	memset((char *)&groupSockAddr, 0, sizeof(groupSockAddr));
	groupSockAddr.sin_family = AF_INET;
	groupSockAddr.sin_addr.s_addr = inet_addr(multicast_ip);
	groupSockAddr.sin_port = htons(multicast_port);

	/* Set local interface for outbound multicast datagrams. */
	localInterface.s_addr = inet_addr(interface_ip);
	if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
		perror("Setting local interface error");
		exit(1);
	}
	std::cout << "Setting the local interface...OK" << std::endl;

	// Publish data
	std::cout << "Sending datagram message..." << std::endl;
	int idx = -1;
	int bytes_sent = 0, total_data_size = 0;
	while (++idx < count)
	{
		bytes_sent = sendto(sockfd, databuf, datalen, 0, (struct sockaddr *)&groupSock, sizeof(groupSock));
		if (bytes_sent < 0)
		{
			perror("Sending datagram message error");
			break;
		}
		total_data_size += bytes_sent;
	}
	std::cout << "Sent " << idx << " datagram messages, total size: " << total_data_size << std::endl;

	return (sockfd);
}
// ------------------------------------------------
static int infd, outfd;

struct io_data
{
	int read;
	off_t first_offset, offset;
	size_t first_len;
	struct iovec iov;
};

static int setup_context(unsigned entries, struct io_uring *ring)
{
	int ret;

	ret = io_uring_queue_init(entries, ring, 0);
	if (ret < 0)
	{
		fprintf(stderr, "queue_init: %s\n", strerror(-ret));
		return -1;
	}

	return 0;
}

static int get_file_size(int fd, off_t *size)
{
	struct stat st;

	if (fstat(fd, &st) < 0)
		return -1;
	if (S_ISREG(st.st_mode))
	{
		*size = st.st_size;
		return 0;
	}
	else if (S_ISBLK(st.st_mode))
	{
		unsigned long long bytes;

		if (ioctl(fd, BLKGETSIZE64, &bytes) != 0)
			return -1;

		*size = bytes;
		return 0;
	}

	return -1;
}

static void queue_prepped(struct io_uring *ring, struct io_data *data)
{
	struct io_uring_sqe *sqe;

	sqe = io_uring_get_sqe(ring);
	assert(sqe);

	if (data->read)
		io_uring_prep_readv(sqe, infd, &data->iov, 1, data->offset);
	else
		io_uring_prep_writev(sqe, outfd, &data->iov, 1, data->offset);

	io_uring_sqe_set_data(sqe, data);
}

static int queue_read(struct io_uring *ring, off_t size, off_t offset)
{
	struct io_uring_sqe *sqe;
	struct io_data *data;

	data = malloc(size + sizeof(*data));
	if (!data)
		return 1;

	sqe = io_uring_get_sqe(ring);
	if (!sqe)
	{
		free(data);
		return 1;
	}

	data->read = 1;
	data->offset = data->first_offset = offset;

	data->iov.iov_base = data + 1;
	data->iov.iov_len = size;
	data->first_len = size;

	io_uring_prep_readv(sqe, infd, &data->iov, 1, offset);
	io_uring_sqe_set_data(sqe, data);
	return 0;
}

static void queue_write(struct io_uring *ring, struct io_data *data)
{
	data->read = 0;
	data->offset = data->first_offset;

	data->iov.iov_base = data + 1;
	data->iov.iov_len = data->first_len;

	queue_prepped(ring, data);
	io_uring_submit(ring);
}

static int copy_file(struct io_uring *ring, off_t insize)
{
	unsigned long reads, writes;
	struct io_uring_cqe *cqe;
	off_t write_left, offset;
	int ret;

	write_left = insize;
	writes = reads = offset = 0;

	while (insize || write_left)
	{
		int had_reads, got_comp;

		/*
		 * Queue up as many reads as we can
		 */
		had_reads = reads;
		while (insize)
		{
			off_t this_size = insize;

			if (reads + writes >= QUEUE_DEPTH)
				break;
			if (this_size > BLOCK_SIZE)
				this_size = BLOCK_SIZE;
			else if (!this_size)
				break;

			if (queue_read(ring, this_size, offset))
				break;

			insize -= this_size;
			offset += this_size;
			reads++;
		}

		if (had_reads != reads)
		{
			ret = io_uring_submit(ring);
			if (ret < 0)
			{
				fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
				break;
			}
		}

		/*
		 * Queue is full at this point. Find at least one completion.
		 */
		got_comp = 0;
		while (write_left)
		{
			struct io_data *data;

			if (!got_comp)
			{
				ret = io_uring_wait_cqe(ring, &cqe);
				got_comp = 1;
			}
			else
			{
				ret = io_uring_peek_cqe(ring, &cqe);
				if (ret == -EAGAIN)
				{
					cqe = NULL;
					ret = 0;
				}
			}
			if (ret < 0)
			{
				fprintf(stderr, "io_uring_peek_cqe: %s\n",
						strerror(-ret));
				return 1;
			}
			if (!cqe)
				break;

			data = io_uring_cqe_get_data(cqe);
			if (cqe->res < 0)
			{
				if (cqe->res == -EAGAIN)
				{
					queue_prepped(ring, data);
					io_uring_cqe_seen(ring, cqe);
					continue;
				}
				fprintf(stderr, "cqe failed: %s\n",
						strerror(-cqe->res));
				return 1;
			}
			else if (cqe->res != data->iov.iov_len)
			{
				/* Short read/write, adjust and requeue */
				data->iov.iov_base += cqe->res;
				data->iov.iov_len -= cqe->res;
				data->offset += cqe->res;
				queue_prepped(ring, data);
				io_uring_cqe_seen(ring, cqe);
				continue;
			}

			/*
			 * All done. if write, nothing else to do. if read,
			 * queue up corresponding write.
			 */
			if (data->read)
			{
				queue_write(ring, data);
				write_left -= data->first_len;
				reads--;
				writes++;
			}
			else
			{
				free(data);
				writes--;
			}
			io_uring_cqe_seen(ring, cqe);
		}
	}

	/* wait out pending writes */
	while (writes)
	{
		struct io_data *data;

		ret = io_uring_wait_cqe(ring, &cqe);
		if (ret)
		{
			fprintf(stderr, "wait_cqe=%d\n", ret);
			return 1;
		}
		if (cqe->res < 0)
		{
			fprintf(stderr, "write res=%d\n", cqe->res);
			return 1;
		}
		data = io_uring_cqe_get_data(cqe);
		free(data);
		writes--;
		io_uring_cqe_seen(ring, cqe);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct io_uring ring;
	off_t insize;
	int ret;

	if (argc < 3)
	{
		printf("%s: infile outfile\n", argv[0]);
		return 1;
	}

	infd = open(argv[1], O_RDONLY);
	if (infd < 0)
	{
		perror("open infile");
		return 1;
	}
	outfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfd < 0)
	{
		perror("open outfile");
		return 1;
	}

	if (setup_context(QUEUE_DEPTH, &ring))
		return 1;
	if (get_file_size(infd, &insize))
		return 1;

	ret = copy_file(&ring, insize);

	close(infd);
	close(outfd);
	io_uring_queue_exit(&ring);
	return ret;
}
