/**_
 * BLUE FIRE CAPITAL, LLC CONFIDENTIAL
 * _____________________
 * 
 *  Copyright © [2007] - [2012] Blue Fire Capital, LLC
 *  All Rights Reserved.
 * 
 * NOTICE:  All information contained herein is, and remains the property
 * of Blue Fire Capital, LLC and its suppliers, if any.  The intellectual
 * and technical concepts contained herein are proprietary to Blue Fire
 * Capital, LLC and its suppliers and may be covered by U.S. and Foreign
 * Patents, patents in process, and are protected by trade secret or copyright
 * law.  Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained from 
 * Blue Fire Capital, LLC.
_**/

/* iouring_msgrecv.hpp: Class implementation to receive multicast messages
 * 
 * Option-1: Create multiple requests, load all to SQE, then submit queue 
 * Option-2: Create multiple iovecs under one call and load/submit queue
 * 
 * g++  -o <app_name> <source.cpp> -luring
 */

#ifndef IOURING_MSGRECV_HPP
#define IOURING_MSGRECV_HPP

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

    #define BUFSIZE 2048

    enum EventType {
        EVENT_TYPE_RECVMSG,
        EVENT_TYPE_SENDMSG
    };

    using MessageHandler = std::function<void (char *, size_t)>;

    struct Request {
        EventType event_type;
        struct msghdr msgs;
        struct iovec iovecs;
        char buff[BUFSIZE + 1];		// consider refactoring to pointer
    };

    class MulticastReceiver {
    private:
        // ----------------------------------------------------
        // class attributes
        const std::string _interface_ip;	
        const std::string _multicast_ip;
        const int _multicast_port;
        struct io_uring _ring;
        struct sockaddr_in _socket_addr;
        struct ip_mreq _group_addr;
        const int _queue_size;
        struct std::unique_ptr<Request[]> _requests;
        int _socketfd;
        bool _run = true;	// run receiver

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
                printf("Interface: %s, Multicast: %s", _interface_ip.c_str(), _multicast_ip.c_str());
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
            printf("Adding multicast group...OK.\n");
        }

        // ----------------------------------------------------
        void SetupContext() {
            struct io_uring_params params;
            memset(&params, 0, sizeof(params));
            params.flags |= IORING_SETUP_SQPOLL;
            params.sq_thread_idle = 20000;		// 20sec idle timeout
            int ret = io_uring_queue_init_params(_queue_size, &_ring, &params);
            // int ret = io_uring_queue_init(_queue_size, &_ring, IORING_SETUP_SQPOLL);
            if (ret < 0) {
                fprintf(stderr, "MulticastReceiver: queue_init: %s\n", strerror(-ret));
                exit(EXIT_FAILURE);
            }
            ret = io_uring_register_files(&_ring, &_socketfd, 1);
            if (ret < 0) {
                fprintf(stderr, "MulticastReceiver: Register files failed: %s\n", 
                    strerror(-ret));
                exit(EXIT_FAILURE);
            }
        }

        // ----------------------------------------------------
        void SetupRequests() {
            for (int idx=0; idx<_queue_size; ++idx) {
                struct Request *req = &_requests[idx];
                req->event_type = EventType::EVENT_TYPE_RECVMSG;
                memset(&req->msgs, 0, sizeof(req->msgs));
                req->iovecs.iov_base = req->buff;
                req->iovecs.iov_len = BUFSIZE;
                req->msgs.msg_iov = &req->iovecs;
                req->msgs.msg_iovlen = 1;
            }
        }

        // ----------------------------------------------------
        void SetupQueueEntry(struct Request *req, unsigned flags, bool submit)  {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&_ring);
            // io_uring_prep_recvmsg(sqe, _socketfd, &(req->msgs), flags);
            io_uring_prep_recvmsg(sqe, 0, &(req->msgs), flags);
            io_uring_sqe_set_flags(sqe, IOSQE_FIXED_FILE);
            io_uring_sqe_set_data(sqe, (void *)req);
            if (submit) io_uring_submit(&_ring);		// need Xple elements to load
        }

    // --------------------------------------------------------
    // public member functions
    public:
        // ----------------------------------------------------
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
        _requests(new Request[queue_size]) {
            SetupSocket();
            SetupContext();
            SetupRequests();
            for (int idx=0; idx<_queue_size; ++idx) {
                SetupQueueEntry(&_requests[idx], 0, idx+1 == _queue_size);
            }
        }

        // ----------------------------------------------------
        // Destructor
        ~MulticastReceiver() {
            fprintf(stdout, "%s: closing iouring queue\n", __func__);
            io_uring_queue_exit(&_ring);
        }

        // ----------------------------------------------------
        // Primary function for mcast data handling
        void StartReceiver(MessageHandler msgHandler) {
            struct io_uring_cqe *cqe;
            printf("%s: Waiting for data\n", __func__);
            while (_run) {
                int ret = io_uring_wait_cqe(&_ring, &cqe);
                struct Request *req = (struct Request *)cqe->user_data;				
                if (ret < 0 || cqe->res < 0) {
                    if (cqe->res < 0) perror("io_uring_wait_cqe");
                    else fprintf(stderr, "Async Request failed: %s for event: %d\n",
                            strerror(cqe->res), static_cast<int>(req->event_type));
                    exit(EXIT_FAILURE);
                }
                /* Mark this completion as seen */
                io_uring_cqe_seen(&_ring, cqe);

                // Set string terminator, not need in my code though.
                char *data = static_cast<char*>(req->iovecs.iov_base);
                msgHandler(data, cqe->res);

                // Now Put it back this entry to the queue
                SetupQueueEntry(req, 0, true);
            }
        }

        // ----------------------------------------------------
        // Force socket to close to stop server immediately.
        void StopReceiver() {
            printf("%s: Stoping server\n", __func__);
            _run = false;
            close(_socketfd);
        }
    };	// class MulticastReceiver
};	// namespace iouring

// ------------------------------------------------
// ****** Example Code ******
// int main(int argc, char *argv[]) {
// 	char interface_ip[] = "127.0.0.1";
// 	char multicast_ip[] = "239.0.0.1";
// 	unsigned short multicast_port = 12345;
// 	const int queue_size = 10;
// 	iouring::MessageHandler messageHandler = [](char *msgData, size_t msgSize) -> void {
// 		printf("%s: Received: %ld bytes: Data: %s\n", __func__, msgSize, msgData);
// 	};
// 	iouring::MulticastReceiver receiver(interface_ip, multicast_ip, multicast_port, queue_size);
// 	receiver.RunServer(messageHandler);
// 	return 0;
// }

#endif // IOURING_MSGRECV_HPP
