/* IouringCommon.h: Common iouring structures */

#ifndef IOURING_COMMON_H
#define IOURING_COMMON_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <functional>

namespace iouring {

#pragma pack(push, 1)
    enum IORING_QUEUE {
        IORING_QUEUE_SIZE = 512
    };

    enum MSG_TYPE {
        MSG_TYPE_SEND,
        MSG_TYPE_RECV,
        MSG_TYPE_ACCEPT,
        MSG_TYPE_END
    };

    enum MSG_BUFF {
        MSG_BUFF_SIZE = 2048,
        MSG_BUFF_SMALL = 1024,
        MSG_BUF_END
    };

    struct Request;
    using MessageHandler = std::function<void (struct Request*, int result)>;

    struct Request {
        int _msg_type;
        int _socketfd;
        MessageHandler _callback;

        Request(int socketfd = 0, int type = MSG_TYPE_END) : 
            _msg_type(type), 
            _socketfd(socketfd) {
        }
    };

    struct AcceptRequest : public Request {
        struct sockaddr_in _addr;
        // struct sockaddr _addr;
        socklen_t _addrlen = sizeof(_addr);
    };

    // iouring prep readv, writev - specialized use only
    // FIXME: consider moving buffer out of request
    struct RwVRequest : public Request {
        struct iovec *_iovecs;
        unsigned _nr_vecs;
        void Init(struct iovec *iv, unsigned nr) {
            _iovecs = iv;
            _nr_vecs = nr;
        }
    };

    // iouring prep recvmsg, sendmsg - buffer managed externally
    struct RwMsgRequest : public Request {
        int _index;                              // user data, datagram index.
        struct msghdr *_msghdr;
        void Init(int type, int sfd, MessageHandler callback, int idx, struct msghdr *msg) {
            _msg_type = type;
            _socketfd = sfd;
            _callback = callback;
            _index = idx;
            _msghdr = msg;
        }
        explicit RwMsgRequest(int socketfd=0) : Request(socketfd) {
        }
    };
#pragma pack(pop)

}	// namespace iouring


#endif // IOURING_COMMON_H
