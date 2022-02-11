// TcpSocket.hpp 
#ifndef IOURING_TCPSOCKET_HPP
#define IOURING_TCPSOCKET_HPP

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
#include <unordered_map>

#include "IoUring.hpp"

// FIXME:
// Formatting: as per nebula structure

namespace iouring {

    class TcpSocket {
        enum {
            SOCKET_NOT_INIT = -1
        };

        int _port;
        u_int32_t _host;
        int _socketfd = SOCKET_NOT_INIT;
        IoUring &_iouring;
        struct RwMsgRequest _req; 
        std::unique_ptr<RwMsgRequest[]> _requests;
        int _msg_count;
        struct AcceptRequest _acceptReq;
        std::unordered_map<int, std::shared_ptr<RwMsgRequest>> _clients; 

        // --------------------------------------------------------------------
        void Init(struct msghdr *msgs) {
            printf("creating socket\n");
            _socketfd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (_socketfd == -1) {
                perror("socket()");
                exit(EXIT_FAILURE);
            }

            // NOTE:
            // User controls the buffers 
            // For recv: Iouring loads and provide index
            // For send: User requires to load data (for sender) and provide index 

            // setup Request buffers   
            for (int idx=0; idx<_msg_count; ++idx) {
                struct RwMsgRequest *req = &_requests[idx];
                // FIXME: Review type and callback init here
                req->Init(MSG_TYPE_END, _socketfd, NULL, idx, &msgs[idx]);
            }
        }

    public:
        // --------------------------------------------------------------------
        TcpSocket(IoUring &iouring, const int port, struct msghdr *msgs, 
                  const int msg_count) : 
                _port(port), 
                _host(htonl(INADDR_ANY)),
                _requests(new RwMsgRequest[msg_count]),
                _msg_count(msg_count),
                _iouring(iouring) {
            Init(msgs);
        }

        // --------------------------------------------------------------------
        TcpSocket(IoUring &iouring, u_int32_t host, const int port, 
                  struct msghdr *msgs, const int msg_count) : 
                _port(port), 
                _host(htonl(host)),
                _requests(new RwMsgRequest[msg_count]),
                _msg_count(msg_count),
                _iouring(iouring) {
            Init(msgs);
        }

        // --------------------------------------------------------------------
        TcpSocket(IoUring &iouring, const char *host, const int port, 
                  struct msghdr *msgs, const int msg_count) : 
                _port(port), 
                _host(inet_addr(host)),
                _requests(new RwMsgRequest[msg_count]),
                _msg_count(msg_count),
                _iouring(iouring) {
            Init(msgs);
        }

        // --------------------------------------------------------------------
        ~TcpSocket() {
            for (auto itr : _clients) close(itr.first);
            close(_socketfd);
        }

        // --------------------------------------------------------------------
        // Only required for IouringPost for older kernels
        int GetNativeSocket() {
            return _socketfd;
        }

        // --------------------------------------------------------------------
        void Bind() {
            printf("binding socket to port: %d\n", _port);
            struct sockaddr_in sa;
            sa.sin_family = AF_INET;
            sa.sin_addr.s_addr = _host;
            sa.sin_port = htons(_port);
            if (::bind(_socketfd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
                perror("bind");
                exit(EXIT_FAILURE);
            }
        }
         
        // --------------------------------------------------------------------
        void Listen(int nClients) {
            printf("listening port: %d\n", _port);
            if (::listen(_socketfd, nClients) < 0) {
                perror("listen");
                exit(EXIT_FAILURE);
            }
        }

        // --------------------------------------------------------------------
        void SetupListener(MessageHandler callback, int nClients=32) {
            SetReuseAddress();
            Bind();
            Listen(nClients);
            AsyncAccept(callback);
        }

        // --------------------------------------------------------------------
        int AddSocket(int socketfd) {
            // FIXME: add error handling, emplace
            _clients.emplace(socketfd, std::make_shared<RwMsgRequest>(socketfd));
            return _clients.size();
        }

        // --------------------------------------------------------------------
        void RemoveSocket(int socketfd) {
            std::cout << __func__ << ": socket id: " << socketfd << std::endl;
            _clients.erase(socketfd);
        }

        // --------------------------------------------------------------------
        // *****  NOTICE: We need to make sure given buffer not submitted to **
        // *****  io ring until all client requests are not processed *********
        // --------------------------------------------------------------------
        // int Broadcast(MessageHandler callback, char *buffer, size_t nbytes) {
        int Broadcast(MessageHandler callback, struct msghdr *msg) {
            // FIXME: use streams instead of old stdio
            printf("%s: clients: %ld\n", __func__, _clients.size());
            for (auto itr : _clients ) {
                itr.second->_msg_type = MSG_TYPE_SEND;
                itr.second->_msghdr = msg;
                itr.second->_callback = callback;   // FIXME: place at init

                _iouring.AsyncSend(itr.second.get());
            }
            _iouring.Submit();    
            return _clients.size();
        }

        // --------------------------------------------------------------------
        // client sockets
        void Connect() {
            printf("connecting socket to port: %d\n", _port);
            struct sockaddr_in sa;
            sa.sin_family = AF_INET;
            sa.sin_addr.s_addr = _host;
            sa.sin_port = htons(_port);
            if (::connect(_socketfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
                perror("connect");
                exit(EXIT_FAILURE);
            }
        }

        // --------------------------------------------------------------------
        // Load all available entries
        void AsyncReceiveAll(MessageHandler callback) {
            for (int idx=0; idx<_msg_count; ++idx) {
                AsyncReceive(callback, idx);
            }
        }

        // --------------------------------------------------------------------
        // Single ring entry
        void AsyncReceive(MessageHandler callback, int index) {
            struct RwMsgRequest *req = &_requests[index];
            req->_msg_type = MSG_TYPE_RECV;
            req->_callback = callback;           // FIXME: should be loaded at init.

            _iouring.AsyncReceive(req);
        }

        // --------------------------------------------------------------------
        // Single ring entry
        void AsyncSend(MessageHandler callback, int index) {
            struct RwMsgRequest *req = &_requests[index];
            req->_msg_type = MSG_TYPE_SEND;
            req->_callback = callback;

            _iouring.AsyncSend(req);
        }

        // --------------------------------------------------------------------
        // server side
        void AsyncAccept(MessageHandler callback) {
            _acceptReq._msg_type = MSG_TYPE_ACCEPT;
            _acceptReq._socketfd = _socketfd;
            _acceptReq._callback = callback;

            _iouring.AsyncAccept(&_acceptReq);
        }

        // --------------------------------------------------------------------
        void SetReuseAddress() {
            int reuse = 1;
            if (setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, 
                    sizeof(reuse)) < 0) {
                perror("setsockopt(SO_REUSEADDR) failed");
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
            printf("Setting socket to reuse address...OK.\n");
        }

        // --------------------------------------------------------------------
        void SetReceiveBuffer(int n = 10*1024*1024) {    
            if(setsockopt(_socketfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) < 0) {
                perror("setsockopt(SO_RCVBUF) failed");
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
            printf("Setting socket receive buffer...OK.\n");
        }

        // --------------------------------------------------------------------
        int GetSocketOption(uint32_t option_type) {
            int option_val; 
            unsigned option_size;
            if(getsockopt(_socketfd, SOL_SOCKET, option_type, &option_val, 
                    &option_size) < 0) {
                perror("setsockopt(SO_RCVBUF) failed");
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
            return option_val;
        }

        // --------------------------------------------------------------------
        void SetSocketOption(uint32_t option_type, int option_val) {
            if(setsockopt(_socketfd, SOL_SOCKET, option_type, &option_val, 
                    sizeof(option_val)) < 0) {
                perror("setsockopt(SO_RCVBUF) failed");
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
        }

    };	// class TcpSocket

}	// namespace iouring


#endif // IOURING_TCPSOCKET_H

