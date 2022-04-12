/* iouring.hpp: Class implementation to send/receive messages using iouring
 * 
 * g++  -o <app_name> <source.cpp> -luring
 */

#ifndef IOURING_IOURING_HPP
#define IOURING_IOURING_HPP

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
#include <string>
#include <memory>
#include <thread>
#include <functional>
#include <iostream>
#include <chrono>

#include <liburing.h>
#include "IoUringCommon.h"

namespace iouring {

    class IoUring {
    private:
        // --------------------------------------------------------------------
        // class attributes
        struct io_uring _ring;
        const int _queue_size;
        bool _run = true;
        bool _pending = false;

    // ------------------------------------------------------------------------
    // non-public helper functions
    private:
        // --------------------------------------------------------------------
        void Init() {
            int ret = io_uring_queue_init(_queue_size, &_ring, 0);
            if (ret < 0) {
                fprintf(stderr, "queue_init: %s\n", strerror(-ret));
                exit(EXIT_FAILURE);
            }
        }

    // ------------------------------------------------------------------------
    // public member functions
    public:
        // --------------------------------------------------------------------
        // Constructor(s)
        explicit IoUring (int queue_size = IORING_QUEUE_SIZE) :
            _queue_size(queue_size) {
            Init();
        }

        // --------------------------------------------------------------------
        // Destructor
        ~IoUring() {
            if (_run) {
                fprintf(stdout, "%s: stopping.\n", __func__);
                Shutdown();
            }
        }

        // --------------------------------------------------------------------
        void AsyncReceive(RwMsgRequest *req, unsigned flags = 0)  {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&_ring);
            if (sqe == NULL) {
                    fprintf(stderr, "sqe is full.\n");
                    exit(EXIT_FAILURE);
            }
            io_uring_prep_recvmsg(sqe, req->_socketfd, req->_msghdr, flags);
            io_uring_sqe_set_data(sqe, (void *)req);
            _pending = true;
        }

        // --------------------------------------------------------------------
        void AsyncReceive(RwVRequest *req, unsigned flags = 0)  {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&_ring);
            if (sqe == NULL) {
                    fprintf(stderr, "sqe is full.\n");
                    exit(EXIT_FAILURE);
            }
            io_uring_prep_readv(sqe, req->_socketfd, req->_iovecs, req->_nr_vecs, flags);
            io_uring_sqe_set_data(sqe, (void *)req);
            _pending = true;
        }

        // --------------------------------------------------------------------
        void AsyncSend(RwMsgRequest *req, unsigned flags = 0)  {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&_ring);
            if (sqe == NULL) {
                    fprintf(stderr, "sqe is full.\n");
                    exit(EXIT_FAILURE);
            }
            io_uring_prep_sendmsg(sqe, req->_socketfd, req->_msghdr, flags);
            io_uring_sqe_set_data(sqe, (void *)req);
            _pending = true;
        }

        // --------------------------------------------------------------------
        void AsyncAccept(AcceptRequest *req, unsigned flags = 0)  {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&_ring);
            if (sqe == NULL) {
                    fprintf(stderr, "sqe is full.\n");
                    exit(EXIT_FAILURE);
            }
            io_uring_prep_accept(sqe, req->_socketfd, (sockaddr *)&req->_addr, &(req->_addrlen), flags);
            io_uring_sqe_set_data(sqe, (void *)req);
            _pending = true;
        }

        // --------------------------------------------------------------------
        void Submit() {
            // printf("DEBUG: submitting sqe...\n");
            io_uring_submit(&_ring);		// need Xple elements to load
            _pending = false;
        }

        // --------------------------------------------------------------------
        void Run() {
            printf("%s: Waiting for data\n", __func__);
            struct io_uring_cqe *cqe;
            struct io_uring_cqe *cqes[_queue_size];
            while (_run) {
                if (_pending) Submit();
                int ret = io_uring_wait_cqe(&_ring, &cqe);
                if (ret < 0) {
                    perror("io_uring_wait_cqe");
                    exit(EXIT_FAILURE);
                }
                // FIXME: Need to handle -104: Connection reset by peer
                int cqe_count = io_uring_peek_batch_cqe(&_ring, cqes, sizeof(cqes)/sizeof(cqes[0]));
                for (int idx=0; idx<cqe_count; ++idx) {
                    cqe = cqes[idx];
                    if (cqe->res < 0 && -cqe->res != EPIPE) {
                        fprintf(stderr, "\nERROR: io_uring_peek_batch_cqe failed: %d: %s\n", 
                                cqe->res, strerror(-cqe->res));
                        exit(EXIT_FAILURE);
                    }
                    Request *req = reinterpret_cast<Request*>(cqe->user_data);
                    io_uring_cqe_seen(&_ring, cqe);		// FIXME: use advance after loop
                    req->_callback(req, cqe->res);
                }
            }
            printf("%s: Done.\n", __func__);
        }

        // --------------------------------------------------------------------
        // Force to stop server immediately.
        void Shutdown() {
            printf(">>> Shutting down iouring.\n");
            if (_run) {
                _run = false;
                io_uring_queue_exit(&_ring);
            }
        }

    };	// class IoUring

}	// namespace iouring


#endif // IOURING_IOURING_HPP
