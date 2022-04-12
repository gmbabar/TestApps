// IouringPost.cpp 
/* 
 * gcc  -o IouringPost IouringPost.cpp -luring
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <memory>
#include <thread>
#include <iostream>
#include <atomic>
#include <vector>

#include "IoUringCommon.h"
#include "IoUring.hpp" 

// FIXME:
// Formatting: as per nebula structure

namespace iouring {

    enum PIPE_IDX {
        PIPE_IDX_READ = 0,
        PIPE_IDX_WRITE,

        PIPE_IDX_END
    };

    enum IOPOST {
        IOPOST_BUFFSIZE = 1024,     // only passing socket info, callbacks
        IOPOST_SOCK_MAX = 128,
        IOPOST_END
    };

    // message post
    class IouringPost {
        iouring::IoUring &_iouring;
        int _pipefds[PIPE_IDX_END];
        char _buffer[IOPOST_BUFFSIZE];
        std::unique_ptr<std::thread> _thread;
        std::atomic<int> _run;
        struct AcceptRequest _acceptReq;
        int _acceptor = -1;

        // concrete RwMsgRequest
        char _iovev_buff[IOPOST_BUFFSIZE];
        struct iovec _iovec;
        struct RwVRequest _rwvReq;

    public:
        IouringPost(iouring::IoUring &iouring) : _iouring(iouring) {
            // RwMsgRequest setup
            _iovec.iov_base = _iovev_buff;
            _iovec.iov_len = sizeof(_iovev_buff);
            _rwvReq.Init(&_iovec, 1);

            // pipe setup
            int ret = pipe(_pipefds);
            if (ret == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            std::cout << __func__ << ": pipes, write: " << _pipefds[PIPE_IDX_WRITE] 
                      << ", read: " << _pipefds[PIPE_IDX_READ] << std::endl;
        }

        int GetWritePipe() { return _pipefds[PIPE_IDX_WRITE]; }
        int GetReadPipe() { return _pipefds[PIPE_IDX_READ]; }

        void AddAcceptSocket(int acceptorfd, MessageHandler callback) {
            _acceptor = acceptorfd;

            _rwvReq._msg_type = MSG_TYPE_RECV;
            _rwvReq._socketfd = GetReadPipe();
            _rwvReq._callback = callback;

            _iouring.AsyncReceive(&_rwvReq);
        }

        static void Run(IouringPost *pthis) {
            std::cout << "IouringPost::run: Started." << std::endl;
            if (pthis->_acceptor == -1) {
                std::cout << "IouringPost::run: No action registered. exiting" << std::endl;
                return;
            }

            struct sockaddr_in _addr;
            socklen_t _addrlen = sizeof(_addr);
            // int counter = 0;
            while (pthis->_run.load()) {
                std::cout << "IouringPost::run: Waiting for clients" << std::endl;
                int ret = ::accept(pthis->_acceptor, (sockaddr*)&_addr, &_addrlen);
                if (ret < 0) {
                    perror("accept error");
                    break;
                }

                std::cout << "IouringPost::run: Received client request from: " 
                          << inet_ntoa(_addr.sin_addr) << ":" 
                          << ntohs(_addr.sin_port) << std::endl;
                pthis->_acceptReq._msg_type = MSG_TYPE_ACCEPT;
                pthis->_acceptReq._socketfd = ret;
                pthis->_acceptReq._callback = NULL;
                pthis->_acceptReq._addr = _addr;
                pthis->_acceptReq._addrlen = _addrlen;
                ::write(pthis->_pipefds[PIPE_IDX_WRITE], &(pthis->_acceptReq), sizeof(pthis->_acceptReq));
            }
            std::cout << "IouringPost::run: Done." << std::endl;
        }

        void Start() {
            _run.store(true);
            _thread.reset(new std::thread(IouringPost::Run, this));
        }

        void Stop() {
            if (_run.load()) _run.store(false);
        }

        ~IouringPost() {
            Stop();
            if (_thread) _thread->join();
        }

    };	// class IouringPost

}	// namespace iouring


// ----------------------------------------------------------------------------
// Test/Sample code
/*
    1. create socket, bind, listen
    2. pass socket to Post by AddAcceptSocket(socketfd, callback)
    3. accept in class Post
    4. write on pipe [fd, callback]
    5. read pipe by iouring and invoke callback()
 */
// ----------------------------------------------------------------------------

#include "TcpSocket.hpp"

int main(int argc, char *argv[]) {
    // structs/vars
	const int queue_size = 16;
    #define BUFFSIZE 256
    struct RecvBuffer {
        char buff[BUFFSIZE];
        struct iovec m_iovec;

        RecvBuffer() {
            m_iovec.iov_base = buff;
            m_iovec.iov_len = sizeof(buff);
        }
    };
    RecvBuffer m_recvbuff[queue_size];
    struct msghdr m_datagrams[queue_size];
    for(int idx = 0; idx < queue_size; ++idx) {
        m_datagrams[idx].msg_iov	  = &(m_recvbuff[idx].m_iovec);
        m_datagrams[idx].msg_iovlen   = 1;
        m_datagrams[idx].msg_name	  = NULL;
        m_datagrams[idx].msg_namelen  = 0;
    }

    // common data.
    char data[128];
    size_t len = sprintf(data, "Test data being passed through tcp socket.");
    char action[16] = "Sent";
    unsigned counter = 0;

    // server data.
    int clients = 0;

    // Iouring
    iouring::IoUring _ioring;
    iouring::TcpSocket tcpSocket(_ioring, 23456, m_datagrams, queue_size);

    // reader/writer callback
    iouring::MessageHandler rwHandler = [&](iouring::Request *req, int msgSize) -> void {
        iouring::RwMsgRequest *rwReq = static_cast<iouring::RwMsgRequest*>(req);
        char *msgData = static_cast<char*>(rwReq->_msghdr->msg_iov->iov_base);
        printf("%s %d bytes: %s - counter: %d \n", action, msgSize, msgData, ++counter);
        // Caution: re-submit should be last operation
        std::cout << __func__ << ": clients left: " << clients << std::endl;
        if (msgSize == -EPIPE) tcpSocket.RemoveSocket(req->_socketfd);

        // mark completion
        if (--clients <= 0) { 
            clients = tcpSocket.Broadcast(rwHandler, &m_datagrams[0]);
        }
        usleep(1000000);    // put some delay
        printf("Message handler complete. - counter: %d\n", counter);    // Important: counter stays 0
    };

    // acceptor
    iouring::MessageHandler acceptHandler = [&](iouring::Request *req, int socketfd) -> void {
        iouring::RwVRequest *rwvReq = static_cast<iouring::RwVRequest*>(req);
        char *msgData = static_cast<char*>(rwvReq->_iovecs->iov_base);
        iouring::AcceptRequest *accReq = reinterpret_cast<iouring::AcceptRequest*>(msgData);
        std::cout << "AcceptHandler: received client socket: " << accReq->_socketfd << std::endl;
        int clients = tcpSocket.AddSocket(accReq->_socketfd);
        strncpy((char*)m_datagrams[0].msg_iov->iov_base, data, len);
        m_datagrams[0].msg_iov->iov_len = len;
        if (clients == 1) tcpSocket.Broadcast(rwHandler, &m_datagrams[0]);		// trigger write for first client
        _ioring.AsyncReceive(rwvReq);
    };

    tcpSocket.SetReuseAddress();
    tcpSocket.Bind();
    tcpSocket.Listen(16);

    iouring::IouringPost post(_ioring);
    post.AddAcceptSocket(tcpSocket.GetNativeSocket(), acceptHandler);
    post.Start();
    _ioring.Run();
    printf("%s: Done.\n", __func__);

    return 0;
}

