// UdpSocket.cpp 

#ifndef IOURING_UDPSOCKET_HPP
#define IOURING_UDPSOCKET_HPP

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
#include <net/if.h>

#include "IoUring.hpp"


namespace iouring {

    class UdpSocket {
        enum {
            SOCKET_NOT_INIT = -1
        };

        int _port;
        u_int32_t _host;
        int _socketfd = SOCKET_NOT_INIT;
        std::unique_ptr<RwMsgRequest[]> _requests;
        int _msg_count;
        IoUring &_iouring;

        // --------------------------------------------------------------------
        void Init(struct msghdr *msgs) {
            printf("creating socket\n");
            _socketfd = ::socket(AF_INET, SOCK_DGRAM, 0);
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
        UdpSocket(IoUring &iouring, const int port, struct msghdr *msgs, 
                  const int msg_count) : 
                _port(port), 
                _host(htonl(INADDR_ANY)),
                _requests(new RwMsgRequest[msg_count]),
                _msg_count(msg_count),
                _iouring(iouring) {
            Init(msgs);
        }

        // --------------------------------------------------------------------
        UdpSocket(IoUring &iouring, u_int32_t host, const int port, 
                  struct msghdr *msgs, const int msg_count) : 
                _port(port), 
                _host(htonl(host)),
                _requests(new RwMsgRequest[msg_count]),
                _msg_count(msg_count),
                _iouring(iouring) {
            Init(msgs);
        }

        // --------------------------------------------------------------------
        UdpSocket(IoUring &iouring, const char *host, const int port, 
                  struct msghdr *msgs, const int msg_count) : 
                _port(port), 
                _host(inet_addr(host)),
                _requests(new RwMsgRequest[msg_count]),
                _msg_count(msg_count),
                _iouring(iouring) {
            Init(msgs);
        }

        // --------------------------------------------------------------------
        ~UdpSocket() {
            close(_socketfd);
        }

        // --------------------------------------------------------------------
        void Bind() {
            printf("binding socket to port: %d\n", _port);
            struct sockaddr_in sa;
            sa.sin_family = AF_INET;
            sa.sin_addr.s_addr = _host;
            sa.sin_port = htons(_port);
            if (::bind(_socketfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
                perror("bind");
                exit(EXIT_FAILURE);
            }
        }

        // --------------------------------------------------------------------
        // NOTE: connect fails if port is not bound by any socket and 
        // iouring doesn't support sendto()
        // Bind is not solution, as it starts buffering data for readq
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
            req->_callback = callback;       // FIXME: should be loaded at init.

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
        }

        // --------------------------------------------------------------------
        // Publisher only
        void SetMulticastInterface(const char *interface_ip) {
            // Set outgoing multicast interface
            struct in_addr interface_addr;
            memset((char *) &interface_addr, 0, sizeof(interface_addr));
            interface_addr.s_addr = inet_addr(interface_ip);
            if(setsockopt(_socketfd, IPPROTO_IP, IP_MULTICAST_IF, 
                         (char*)&interface_addr, sizeof(interface_addr)) < 0) {
                perror("setsocketopt-IP_MULTICAST_IF");
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
            printf("%s: multicast publiser interface is set: %s\n", 
                   __func__, interface_ip);
        }

        // --------------------------------------------------------------------
        void SetMulticastTtl(unsigned char ttl = 1) {
            // Set multicast ttl
            if(setsockopt(_socketfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
                std::cerr << "Failed to set publish ttl!" << std::endl;
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
        }

        // --------------------------------------------------------------------
        // Subscriber only
        void SubscribeMulticastGroup(const char *multicast_ip, const char *interface_ip) {
            // Subscribe to multicast group
            struct ip_mreqn group;
            group.imr_multiaddr.s_addr = inet_addr(multicast_ip);
            group.imr_address.s_addr = htonl(INADDR_ANY);
            if(interface_ip != NULL && strlen(interface_ip) > 0)
                group.imr_ifindex = if_nametoindex(interface_ip);
            else
                group.imr_ifindex = 0;
            if(setsockopt(_socketfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
                std::cerr << "Failed to add subscription to multicast group!" << std::endl;
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
            std::cout << __func__ << ": Joined udp multicast=" << multicast_ip 
                << " interface=" << interface_ip << std::endl;
        }

        // --------------------------------------------------------------------
        int GetSocketOption(uint32_t option_type) {
            int option_val = 0;
            unsigned option_size = 0;
            if(getsockopt(_socketfd, SOL_SOCKET, option_type, &option_val, 
                    &option_size) < 0) {
                perror("setsockopt(SO_RCVBUF) failed");
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
            return option_val;
        }

        // --------------------------------------------------------------------
        void SetSocketOptions(uint32_t option_type, int option_val) {
            if(setsockopt(_socketfd, SOL_SOCKET, option_type, &option_val, 
                    sizeof(option_val)) < 0) {
                perror("setsockopt(SO_RCVBUF) failed");
                close(_socketfd);
                exit(EXIT_FAILURE);
            }
        }

    };	// class UdpSocket

}	// namespace iouring


#endif  // IOURING_UDPSOCKET_H
