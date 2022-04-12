/* iouring_msgsend.hpp: 
 *
 * Class implementation to publish multicast messages with liburing
 *  
 * g++  -o <app_name> <source.cpp> -luring
 */

#ifndef IOURING_MSGSEND_HPP
#define IOURING_MSGSEND_HPP

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

#include <liburing.h>

namespace iouring {

    struct SendRequest {
        struct msghdr msgs;
        struct iovec iovecs;
    };

    #define BATCH_SIZE 10

    class MulticastPublisher {
    private:
        // ----------------------------------------------------
        // class attributes
        const std::string _interface_ip;	
        const std::string _multicast_ip;
        const int _multicast_port;
        struct io_uring _ring;
        struct sockaddr_in _socket_addr;
        struct in_addr _local_interface;
        const int _queue_size;
        struct std::unique_ptr<SendRequest[]> _requests;
        int _socketfd;
        bool _run = true;	// run publisher
        int _sqe_index = 0;

    // --------------------------------------------------------
    // non-public helper functions
    private:
        // ----------------------------------------------------
        void SetupSocket()  {
            _socketfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_socketfd == -1) {
                perror("socket() failed");
                exit(EXIT_FAILURE);
            }

            {
                /* Set local interface for outbound multicast datagrams. */
                /* The IP address specified must be associated with a local, */
                /* multicast capable interface. */
                _local_interface.s_addr = inet_addr(_interface_ip.c_str());
                if (setsockopt(_socketfd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&_local_interface, sizeof(_local_interface)) < 0) {
                    perror("Setting local interface error");
                    exit(1);
                }
                printf("Setting the local interface...OK\n");
            }

            /* Bind to the proper port number with the IP address */
            memset((char *)&_socket_addr, 0, sizeof(_socket_addr));
            _socket_addr.sin_family = AF_INET;
            _socket_addr.sin_addr.s_addr = inet_addr(_multicast_ip.c_str());
            _socket_addr.sin_port = htons(_multicast_port);
            if (connect(_socketfd, (struct sockaddr *)&_socket_addr, sizeof(_socket_addr)) == -1) {
                perror("connect() failed:");
                exit(EXIT_FAILURE);
            }
            printf("connect datagram socket...OK.\n");

            // Set multicast ttl
            unsigned char ttl = 32;
            if(setsockopt(_socketfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
                std::cerr << "Failed to set publish ttl!" << std::endl;
                exit(EXIT_FAILURE);
            }

        }

        // ----------------------------------------------------
        void SetupContext() {
            int ret = io_uring_queue_init(_queue_size, &_ring, 0);
            if (ret < 0) {
                fprintf(stderr, "queue_init: %s\n", strerror(-ret));
                exit(EXIT_FAILURE);
            }
        }

        // ------------------------------------------------
        SendRequest* SetupRequest(char *buff, size_t length)
        {
            if (++_sqe_index == _queue_size) _sqe_index = 0;
            SendRequest *req = &_requests[_sqe_index];
            memset(&req->msgs, 0, sizeof(req->msgs));
            req->iovecs.iov_base = buff;
            req->iovecs.iov_len = length;
            req->msgs.msg_iov = &req->iovecs;
            req->msgs.msg_iovlen = 1;
            return req;
        }

        // ----------------------------------------------------
        void SetupQueueEntry(SendRequest *req, unsigned flags, bool submit)  {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&_ring);
            io_uring_prep_sendmsg(sqe, _socketfd, &(req->msgs), flags);
            io_uring_sqe_set_data(sqe, (void *)req);
            if (submit) io_uring_submit(&_ring);
        }

    // --------------------------------------------------------
    // public member functions
    public:
        // ----------------------------------------------------
        // Constructor(s)
        MulticastPublisher (
            const std::string &interface_ip,
            const std::string &multicast_ip,
            const int multicast_port,
            const int queue_size	
        ) : _interface_ip(interface_ip),
        _multicast_ip(multicast_ip),
        _multicast_port(multicast_port),
        _queue_size(queue_size),
        _requests(new SendRequest[queue_size]) {
            SetupSocket();
            SetupContext();
        }

        // ----------------------------------------------------
        // Destructor
        ~MulticastPublisher() {
            fprintf(stdout, "%s: closing iouring queue\n", __func__);
            io_uring_queue_exit(&_ring);
        }

        // ----------------------------------------------------
        void SendMessage(char *buff, size_t length) {
            // create ring entry and submit to the queue
            SendRequest *req = SetupRequest(buff, length);
            SetupQueueEntry(req, 0, true);

            struct io_uring_cqe *cqe = NULL;
            struct io_uring_cqe *cqes[BATCH_SIZE];     // batch receive 
            // printf("%s: publishing data\n", __func__);
            {
                // int ret = io_uring_wait_cqe(&_ring, &cqe);
                int ret = io_uring_peek_cqe(&_ring, &cqe);
                // if (ret < 0) {
                //      perror("io_uring_wait_cqe");
                // 	exit(EXIT_FAILURE);
                // }
                int cqe_count = io_uring_peek_batch_cqe(&_ring, cqes, _queue_size);
                for (int idx=0; idx<cqe_count; ++idx) {
                    cqe = cqes[idx];
                    /* Mark this completion as seen */
                    io_uring_cqe_seen(&_ring, cqe);   
                }
            }
        }
    };	// class MulticastPublisher
};	// namespace iouring

// // ------------------------------------------------
// // ****** Example Code ******
// int main(int argc, char *argv[]) {
// 	char interface_ip[] = "127.0.0.1";
// 	char multicast_ip[] = "239.0.0.1";
// 	unsigned short multicast_port = 12345;
// 	const int queue_size = 10;
// 	char buffer[256];
// 	int len = 0;
// 	iouring::MulticastPublisher publisher(interface_ip, multicast_ip, multicast_port, queue_size);
// 	for (int idx = 0; idx < queue_size; ++idx) {
// 		len = sprintf(buffer, "%d: This is io_uring_sendmsg test, time: %ld", idx, time(NULL));
// 		publisher.SendMessage(buffer, len+1);
// 	}
// 	return 0;
// }

#endif // IOURING_MSGSEND_HPP
