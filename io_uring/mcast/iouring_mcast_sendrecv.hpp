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
    #define BATCH_SIZE 32

    enum EventType {
        EVENT_TYPE_RECVMSG,
        EVENT_TYPE_SENDMSG
    };

    enum SocketIndex {
        SOCK_IDX_RECV = 0,
        SOCK_IDX_SEND,

        SOCK_IDX_COUNT
    };

    using MessageHandler = std::function<void (char *, size_t)>;

    struct Request {
        EventType event_type;
        struct msghdr msgs;
        struct iovec iovecs;
        bool is_used;
        char buff[BUFSIZE];		// consider refactoring to pointer
    };

    class MulticastSendRecv {
    private:
        // ----------------------------------------------------
        // class attributes
        const std::string _interface_ip;	
        const std::string _recv_multicast_ip;
        const std::string _send_multicast_ip;
        const int _recv_multicast_port;
        const int _send_multicast_port;
        struct io_uring _ring;
        struct ip_mreq _group_addr;
        const int _queue_size;      // ioring size
        const int _recv_size;       // slots for recv entries within ioring
        const int _send_size;       // slots for send entries within ioring
        int _send_index;      // current idx pointing to send entries
        struct std::unique_ptr<Request[]> _recv_requests;
        struct std::unique_ptr<Request[]> _send_requests;
        int _socketfd[SOCK_IDX_COUNT];
        bool _run = true;	// run receiver

    // --------------------------------------------------------
    // non-public helper functions
    private:
        // ----------------------------------------------------
        void SetupRecvSocket()  {
            _socketfd[SOCK_IDX_RECV] = socket(AF_INET, SOCK_DGRAM, 0);
            if (_socketfd[SOCK_IDX_RECV] == -1) {
                perror("socket() failed");
                exit(EXIT_FAILURE);
            }

            /* Enable SO_REUSEADDR to allow multiple instances to run */
            {
                int reuse = 1;
                if (setsockopt(_socketfd[SOCK_IDX_RECV], SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
                {
                    perror("setsockopt(SO_REUSEADDR) failed");
                    close(_socketfd[SOCK_IDX_RECV]);
                    exit(1);
                }
                printf("%s: Setting socket to reuse address...OK.\n", __func__);
            }

            // Set RCVBUF option 
            {    
            	int n = 1024*1024*10;
            	if(setsockopt(_socketfd[SOCK_IDX_RECV], SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) < 0) {
            		std::cerr << "Failed to set receive socket buffer size!" << std::endl;
            		close(_socketfd[SOCK_IDX_RECV]);
            		exit(EXIT_FAILURE);
            	}
            }

            /* Bind to the proper port number with the IP address */
            struct sockaddr_in socket_addr;
            memset((char *)&socket_addr, 0, sizeof(socket_addr));
            socket_addr.sin_family = AF_INET;
            socket_addr.sin_addr.s_addr = INADDR_ANY;
            socket_addr.sin_port = htons(_recv_multicast_port);
            if (bind(_socketfd[SOCK_IDX_RECV], (struct sockaddr *)&socket_addr, sizeof(socket_addr)) == -1) {
                perror("bind() failed:");
                exit(EXIT_FAILURE);
            }
            printf("%s: Binding datagram socket...OK.\n", __func__);

            /* Join the multicast group */
            _group_addr.imr_multiaddr.s_addr = inet_addr(_recv_multicast_ip.c_str());
            _group_addr.imr_interface.s_addr = inet_addr(_interface_ip.c_str());
            if (setsockopt(_socketfd[SOCK_IDX_RECV], IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&_group_addr, sizeof(_group_addr)) < 0) {
                perror("Multicast setsockopt failed");
                fprintf(stderr, "%s: Interface: %s, Multicast: %s", __func__, _interface_ip.c_str(), _recv_multicast_ip.c_str());
                close(_socketfd[SOCK_IDX_RECV]);
                exit(EXIT_FAILURE);
            }
            printf("%s: Adding multicast group...OK.\n", __func__);
        }

        // ----------------------------------------------------
        void SetupSendSocket()  {
            _socketfd[SOCK_IDX_SEND] = socket(AF_INET, SOCK_DGRAM, 0);
            if (_socketfd[SOCK_IDX_SEND] == -1) {
                perror("socket() failed");
                exit(EXIT_FAILURE);
            }

            {
                /* Set local interface for outbound multicast datagrams. */
                /* The IP address specified must be associated with a local, */
                /* multicast capable interface. */
                struct in_addr interface;
                interface.s_addr = inet_addr(_interface_ip.c_str());
                if (setsockopt(_socketfd[SOCK_IDX_SEND], IPPROTO_IP, IP_MULTICAST_IF, 
                        (char *)&interface, sizeof(interface)) < 0) {
                    perror("Setting local interface error");
                    exit(1);
                }
                printf("%s: Setting the local interface...OK\n", __func__);
            }

            /* Bind to the proper port number with the IP address */
            struct sockaddr_in socket_addr;
            memset((char *)&socket_addr, 0, sizeof(socket_addr));
            socket_addr.sin_family = AF_INET;
            socket_addr.sin_addr.s_addr = inet_addr(_send_multicast_ip.c_str());
            socket_addr.sin_port = htons(_send_multicast_port);
            if (connect(_socketfd[SOCK_IDX_SEND], (struct sockaddr *)&socket_addr, 
                    sizeof(socket_addr)) == -1) {
                perror("connect() failed:");
                exit(EXIT_FAILURE);
            }
            printf("%s: Connect datagram socket...OK.\n", __func__);

            // Set multicast ttl
            unsigned char ttl = 32;
            if(setsockopt(_socketfd[SOCK_IDX_SEND], IPPROTO_IP, IP_MULTICAST_TTL, &ttl, 
                    sizeof(ttl)) < 0) {
                std::cerr << "Failed to set publish ttl!" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        // ----------------------------------------------------
        void SetupRingContext() {
            struct io_uring_params params;
            memset(&params, 0, sizeof(params));
            params.flags |= IORING_SETUP_SQPOLL;
            params.sq_thread_idle = 20000;		// 20sec idle timeout
            int ret = io_uring_queue_init_params(_queue_size, &_ring, &params);
            // int ret = io_uring_queue_init(_queue_size, &_ring, IORING_SETUP_SQPOLL);
            if (ret < 0) {
                fprintf(stderr, "MulticastSendRecv: queue_init: %s\n", strerror(-ret));
                exit(EXIT_FAILURE);
            }
            ret = io_uring_register_files(&_ring, _socketfd, 2);
            if (ret < 0) {
                fprintf(stderr, "MulticastSendRecv: Register files failed: %s\n", 
                    strerror(-ret));
                exit(EXIT_FAILURE);
            }
        }

        // ----------------------------------------------------
        void SetupReceiveRequests() {
            for (int idx=0; idx<_recv_size; ++idx) {
                Request *req = &_recv_requests[idx];
                req->event_type = EventType::EVENT_TYPE_RECVMSG;
                memset(&req->msgs, 0, sizeof(req->msgs));
                req->iovecs.iov_base = req->buff;
                req->iovecs.iov_len = BUFSIZE;
                req->msgs.msg_iov = &req->iovecs;
                req->msgs.msg_iovlen = 1;
            }
        }

        // ------------------------------------------------
        Request* SetupSendRequest(char *buff, size_t length) {
            Request *req = &_send_requests[_send_index++];
            if (req->is_used) {
                fprintf(stderr, "FATAL: Send queue is full\n");
                exit(EXIT_FAILURE);
            }
            _send_index %= _recv_size;

            memset(&req->msgs, 0, sizeof(req->msgs));
            req->event_type = EventType::EVENT_TYPE_SENDMSG;
            req->is_used = true;
            memcpy(req->buff, buff, length);
            req->iovecs.iov_base = req->buff;
            req->iovecs.iov_len = length;
            req->msgs.msg_iov = &req->iovecs;
            req->msgs.msg_iovlen = 1;
            return req;
        }

        // ----------------------------------------------------
        void SetupReceiveQueueEntry(Request *req, unsigned flags, bool submit)  {
            struct io_uring_sqe *sqe = GetQueueEntry();
            // io_uring_prep_recvmsg(sqe, _socketfd, &(req->msgs), flags);
            io_uring_prep_recvmsg(sqe, SOCK_IDX_RECV, &(req->msgs), flags);
            io_uring_sqe_set_flags(sqe, IOSQE_FIXED_FILE);
            io_uring_sqe_set_data(sqe, (void *)req);
            if (submit) io_uring_submit(&_ring);		// need Xple elements to load
        }

        // ----------------------------------------------------
        void SetupSendQueueEntry(Request *req, unsigned flags, bool submit)  {
            struct io_uring_sqe *sqe = GetQueueEntry();
            // io_uring_prep_sendmsg(sqe, _socketfd, &(req->msgs), flags);
            io_uring_prep_sendmsg(sqe, SOCK_IDX_SEND, &(req->msgs), flags);
            io_uring_sqe_set_flags(sqe, IOSQE_FIXED_FILE);
            io_uring_sqe_set_data(sqe, (void *)req);
            if (submit) io_uring_submit(&_ring);
        }

        // ----------------------------------------------------
        inline io_uring_sqe* GetQueueEntry()  {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&_ring);
            unsigned counter = 0;
            while(!sqe) {
                if (counter++ % 1000 == 0)
                    fprintf(stderr, "ERROR: No sqe entry left, waiting for io to finish\n");
                usleep(10);
                sqe = io_uring_get_sqe(&_ring);
            }
            return sqe;
        }

    // --------------------------------------------------------
    // public member functions
    public:
        // ----------------------------------------------------
        // Constructor(s)
        MulticastSendRecv (
            const std::string &interface_ip,
            const std::string &recv_mcast_ip,
            const std::string &send_mcast_ip,
            const int recv_mcast_port,
            const int send_mcast_port,
            const int recv_slots,	
            const int send_slots
        ) : _interface_ip(interface_ip),
        _recv_multicast_ip(recv_mcast_ip),
        _send_multicast_ip(send_mcast_ip),
        _recv_multicast_port(recv_mcast_port),
        _send_multicast_port(send_mcast_port),
        _queue_size(recv_slots + send_slots),
        _recv_size(recv_slots),
        _send_size(recv_slots),
        _recv_requests(new Request[_recv_size]),
        _send_requests(new Request[_send_size]) {
            _send_index = _recv_size;
            SetupSendSocket();
            SetupRecvSocket();
            SetupRingContext();
            SetupReceiveRequests();
            for (int idx=0; idx<_recv_size; ++idx) {
                SetupReceiveQueueEntry(&_recv_requests[idx], 0, idx+1 == _recv_size);
            }
        }

        // ----------------------------------------------------
        // Destructor
        ~MulticastSendRecv() {
            fprintf(stdout, "%s: closing iouring queue\n", __func__);
            io_uring_queue_exit(&_ring);
        }

        // ----------------------------------------------------
        // Primary function for mcast data handling
        void StartReceiver(MessageHandler msgHandler) {
            struct io_uring_cqe *cqe;
            struct io_uring_cqe *cqes[BATCH_SIZE];     // batch receive 

            printf("%s: Waiting for data\n", __func__);
            while (_run) {
                int ret = io_uring_wait_cqe(&_ring, &cqe);
                if (ret < 0) {
                    perror("io_uring_wait_cqe");
                    exit(EXIT_FAILURE);
                }

				// check how many msgs are available:
                int cqe_count = io_uring_peek_batch_cqe(&_ring, cqes, _queue_size);
                for (int idx=0; idx<cqe_count; ++idx) {
                    cqe = cqes[idx];
                    struct Request *req = (struct Request *)cqe->user_data;

                    /* Mark this completion as seen */
                    io_uring_cqe_seen(&_ring, cqe);   

                    if (req->event_type == EVENT_TYPE_SENDMSG) {
                        req->is_used = false;
                        continue;
                    }

                    msgHandler(static_cast<char*>(req->iovecs.iov_base), cqe->res);
                    SetupReceiveQueueEntry(req, 0, idx+1 == cqe_count);
                }
            }
        }

        // ----------------------------------------------------
        void SendMessage(char *buff, size_t length) {
            // create ring entry and submit to the queue
            Request *req = SetupSendRequest(buff, length);
            SetupSendQueueEntry(req, 0, true);
        }

        // ----------------------------------------------------
        // Force socket to close to stop server immediately.
        void StopReceiver() {
            printf("%s: Stoping server\n", __func__);
            _run = false;
            exit(EXIT_SUCCESS);
            // close(_socketfd);
        }
    };	// class MulticastSendRecv
};	// namespace iouring

// ------------------------------------------------
// ****** Example Code ******
// int main(int argc, char *argv[]) {
// 	char interface_ip[] = "127.0.0.1";
// 	char multicast_send_ip[] = "239.0.0.1";
// 	char multicast_recv_ip[] = "239.0.0.1";
// 	unsigned short multicast_recv_port = 12345;
// 	unsigned short multicast_send_port = 23456;
//     std::size_t recv_queue = 80;
//     std::size_t send_queue = 64;
//     iouring::MulticastSendRecv sub_pub(
//         interface_ip, 
//         multicast_recv_ip, 
//         multicast_send_ip, 
//         multicast_recv_port, 
//         multicast_send_port, 
//         recv_queue, 
//         send_queue);
// 	iouring::MessageHandler messageHandler = [](char *msgData, size_t msgSize) -> void {
// 		// printf("%s: Received: %ld bytes: Data: %s\n", __func__, msgSize, msgData);
//         sub_pub.SendMessage(msgData, msgSize);
// 	};
// 	sub_pub.StartReceiver(messageHandler);
// 	return 0;
// }

#endif // IOURING_MSGRECV_HPP
