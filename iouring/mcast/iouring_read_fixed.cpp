/* mcastrecv_iouring
 *	 Test code to send multiple messages.
 * Option-1: Create multiple requests, submit all to SQE, then submit queue 
 * Option-2: Create multiple iovecs under one call
 * 
 * gcc  -o mcastrecv_iouring mcastrecv_iouring.c -luring
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
#include <string>
#include <memory>
#include <functional>
#include <iostream>
#include <chrono>

#include <liburing.h>

#define BUFSIZE 256
#define QSIZE 32
// struct iovec _iovecs[QSIZE];

namespace iouring {


	enum EventType {
		EVENT_TYPE_RECVMSG,
		EVENT_TYPE_SENDMSG
	};

	using MessageHandler = std::function<void (char *, size_t)>;

	struct Request {
		// EventType event_type;
		// struct msghdr msgs;
		// struct iovec iovecs;
		char buff[BUFSIZE];		// consider refactoring to pointer
		int qindex;
	};

	class MulticastReceiver {
	private:
		// ------------------------------------------------
		// class attributes
		const std::string _interface_ip;	
		const std::string _multicast_ip;
		const int _multicast_port;
		struct io_uring _ring;
		struct sockaddr_in _socket_addr;
		struct ip_mreq _group_addr;
		const int _queue_size;
		std::unique_ptr<Request[]> _requests;
		std::unique_ptr<struct iovec[]> _iovecs;
		int _socketfd;
		bool _run = true;

	private:
		// ------------------------------------------------
		// non-public helper functions
		void SetupSocket()  {
			_socketfd = socket(AF_INET, SOCK_DGRAM, 0);
			if (_socketfd == -1) {
				perror("socket() failed");
				exit(EXIT_FAILURE);
			}

			/* Enable SO_REUSEADDR to allow multiple instances to run */
			{
				int reuse = 1;
				if (setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
				{
					perror("setsockopt(SO_REUSEADDR) failed");
					close(_socketfd);
					exit(1);
				}
				printf("Setting socket to reuse address...OK.\n");
			}

			/* Bind to the proper port number with the IP address */
			memset((char *)&_socket_addr, 0, sizeof(_socket_addr));
			_socket_addr.sin_family = AF_INET;
			_socket_addr.sin_addr.s_addr = INADDR_ANY;
			_socket_addr.sin_port = htons(_multicast_port);
			if (bind(_socketfd, (struct sockaddr *)&_socket_addr, sizeof(_socket_addr)) == -1) {
				perror("bind() failed:");
				exit(EXIT_FAILURE);
			}
			printf("Binding datagram socket...OK.\n");

			/* Join the multicast group */
			_group_addr.imr_multiaddr.s_addr = inet_addr(_multicast_ip.c_str());
			_group_addr.imr_interface.s_addr = inet_addr(_interface_ip.c_str());
			if (setsockopt(_socketfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&_group_addr, sizeof(_group_addr)) < 0) {
				perror("Multicast setsockopt failed");
				close(_socketfd);
				exit(EXIT_FAILURE);
			}
			printf("Adding multicast group...OK.\n");
		}

		void SetupContext() {
			printf("%s\n", __func__);
            struct io_uring_params params;
            memset(&params, 0, sizeof(params));
            params.flags |= IORING_SETUP_SQPOLL;
            params.sq_thread_idle = 20000;		// 20sec idle timeout
            int ret = io_uring_queue_init_params(_queue_size, &_ring, &params);

			// int ret = io_uring_queue_init(_queue_size, &_ring, 0);
			if (ret < 0) {
				fprintf(stderr, "queue_init: %s\n", strerror(-ret));
				exit(EXIT_FAILURE);
			}

            ret = io_uring_register_files(&_ring, &_socketfd, 1);
            if (ret < 0) {
                fprintf(stderr, "MulticastReceiver: Register files failed: %s\n", 
                    strerror(-ret));
                exit(EXIT_FAILURE);
            }
		}

		void SetupRequests() {
			printf("%s\n", __func__);
			for (int idx=0; idx<_queue_size; ++idx) {
				struct Request *req = &_requests[idx];
				struct iovec *iov = &_iovecs[idx];
				req->qindex = idx;
				iov->iov_base = req->buff;
				iov->iov_len = BUFSIZE;
			}

			int ret = io_uring_register_buffers(&_ring, _iovecs.get(), _queue_size);
			if (ret) {
				fprintf(stderr, "Failed registering buffers: %s\n.", strerror(-ret));
				exit(EXIT_FAILURE);
			}
		}

		void SetupQueueEntry(struct Request *req, unsigned flags, bool submit)  {
			struct io_uring_sqe *sqe = io_uring_get_sqe(&_ring);

			// io_uring_prep_read_fixed(sqe, _socketfd, req->buff, BUFSIZE, flags, req->qindex);
			/**** sqe polling ****/
			io_uring_prep_read_fixed(sqe, 0, req->buff, BUFSIZE, flags, req->qindex);
            io_uring_sqe_set_flags(sqe, IOSQE_FIXED_FILE);

			io_uring_sqe_set_data(sqe, (void *)req);
			if (submit) io_uring_submit(&_ring);		// need Xple elements to load
		}

	// ------------------------------------------------
	// public functions
	public:
		// ------------------------------------------------
		// Constructor(s)
		MulticastReceiver (
			const std::string &interface_ip,
			const std::string &multicast_ip,
			const int multicast_port,
			const int queue_size	
		) : _interface_ip(interface_ip),
		_multicast_ip(multicast_ip),
		_multicast_port(multicast_port),
		_queue_size(queue_size),
		_requests(new Request[queue_size]),	
		_iovecs(new struct iovec[queue_size]) 
		{
			SetupSocket();
			SetupContext();
			SetupRequests();
			for (int idx=0; idx<_queue_size; ++idx) {
				SetupQueueEntry(&_requests[idx], 0, idx+1 == _queue_size);
			}
		}

		// ------------------------------------------------
		// Destructor
		~MulticastReceiver() {
			fprintf(stdout, "%s: closing iouring queue\n", __func__);
			io_uring_queue_exit(&_ring);
		}

		// ------------------------------------------------
		// Primary function for mcast data handling
		void StartReceiver(MessageHandler msgHandler) {
			struct io_uring_cqe *cqe;
			printf("%s: Waiting for data\n", __func__);
// long int divide = std::chrono::system_clock::period::den / 1000000;
// long long total_duration = 0, start_time, end_time;
// long count = 0;

			while (_run) {
				int ret = io_uring_wait_cqe(&_ring, &cqe);
                if (ret < 0) {
                    perror("io_uring_wait_cqe");
                    exit(EXIT_FAILURE);
                }

				// // Set string terminator, not need in my code though.
				// char *data = static_cast<char*>(req->buff);
				// data[cqe->res] = 0;
				// msgHandler(data, cqe->res);

				// check how many msgs are available:
                struct io_uring_cqe *cqes[20];
                int cqe_count = io_uring_peek_batch_cqe(&_ring, cqes, sizeof(cqes)/sizeof(cqes[0]));
                for (int i=0; i<cqe_count; ++i) {
                    cqe = cqes[i];
                    struct Request *req = (struct Request *)cqe->user_data;

                    io_uring_cqe_seen(&_ring, cqe);   
                    char *data = static_cast<char*>(req->buff);
                    msgHandler(data, cqe->res);
                    SetupQueueEntry(req, 0, i+1 == cqe_count);
                }


// 	// Duration.
// 	end_time = static_cast<long int>(std::chrono::system_clock::now().time_since_epoch().count() / divide);
// 	start_time = strtoll(data, NULL, 10);
// 	total_duration += (end_time - start_time);
// 	++count;
// if (count%1000 == 0)
// 	std::cout << "Average duration: " << static_cast<int>(total_duration / count) << " microseconds" << std::endl;


				// // Now Put it back this entry to the queue
                // static long counter = 0;
                // SetupQueueEntry(req, 0, counter % 10 == 0);
				// // SetupQueueEntry(req, 0, true);
			}
		}

		// --------------------------------------------------
		// Force socket to close to stop server immediately.
		void StopReceiver() {
			_run = false;
			close(_socketfd);
		}
	};	// class MulticastReceiver
};	// namespace iouring

namespace {		// Capture by reference on lambda from c-style singal handler
        std::function<void(int)> shutdown_server;
        void signal_handler(int signal) {
                printf("%s: Caught signal: SIGINT \n", __func__);
                shutdown_server(signal);
        }
}

// ------------------------------------------------
int main(int argc, char *argv[]) {
	char interface_ip[] = "127.0.0.1";
	char multicast_ip[] = "239.0.0.1";
	unsigned short multicast_port = 12345;
	const int queue_size = QSIZE;

    long int divide = std::chrono::system_clock::period::den / 1000000;
    long long total_duration = 0, start_time, end_time;
    long count = 0;

	iouring::MessageHandler messageHandler = [&](char *msgData, size_t msgSize) -> void {
        // Duration.
        end_time = static_cast<long int>(std::chrono::system_clock::now().time_since_epoch().count() / divide);
        start_time = strtoll(msgData, NULL, 10);
        total_duration += (end_time - start_time);
        if (++count%1000 == 0)
	        std::cout << "Average duration: " << static_cast<int>(total_duration / count) << " microseconds" << std::endl;
	};

	iouring::MulticastReceiver receiver(interface_ip, multicast_ip, multicast_port, queue_size);

	// Wait for when ctrl-c (SIGINT) to stop receiver
	shutdown_server = [&](int signum) {
			printf("%s: Stoping server \n", __func__);
			receiver.StopReceiver();
			printf("%s: Done. \n", __func__);
			exit(EXIT_SUCCESS);
	};
	signal(SIGINT, signal_handler); // val: 2 - ctrl+c

	receiver.StartReceiver(messageHandler);

	return 0;
}

