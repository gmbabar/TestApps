// TcpSocket.cpp 
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

#include "TcpSocket.hpp"

// ----------------------------------------------------------------------------
// Test/Sample code
int main(int argc, char *argv[]) {
    bool server_mode = true;     // default

    if (argc == 2 && !strcmp(argv[1], "-c")) {
        server_mode = false;
    } else if ( argc != 1 && strcmp(argv[1], "-s") ) {
        printf("\nInvalid arguments.\n");
        printf("%s [-c|-s] \n", argv[0]);
        return 0;
    }

	const int queue_size = 16;

    struct RecvBuffer {
        char buff[iouring::MSG_BUFF_SMALL];
        struct iovec m_iovec;

        RecvBuffer() {
            m_iovec.iov_base = buff;
            m_iovec.iov_len = sizeof(buff);
        }
    };

    RecvBuffer m_recvbuff[queue_size];
    struct msghdr m_datagrams[queue_size];
    for(int idx = 0; idx < queue_size; ++idx) {
        m_datagrams[idx].msg_iov	= &(m_recvbuff[idx].m_iovec);
        m_datagrams[idx].msg_iovlen   = 1;
        m_datagrams[idx].msg_name	= NULL;
        m_datagrams[idx].msg_namelen  = 0;
    }

    iouring::IoUring _ioring;
    iouring::TcpSocket tcpSocket(_ioring, 23456, m_datagrams, queue_size);
    char action[16] = "Sent";
    unsigned counter = 0;

    // common data.
    char data[iouring::MSG_BUFF_SMALL];
    size_t len = sprintf(data, "Test data being passed through tcp socket.");


    // server data.
    int clients = 0;

    // reader/writer callback
    iouring::MessageHandler rwHandler = [&](iouring::Request *req, int msgSize) -> void {
        iouring::RwMsgRequest *rwReq = static_cast<iouring::RwMsgRequest*>(req);
        char *msgData = static_cast<char*>(rwReq->_msghdr->msg_iov->iov_base);
        printf("%s %d bytes: %s - counter: %d \n", action, msgSize, msgData, ++counter);
        if (!server_mode && counter == 100) {
            _ioring.Shutdown();
            printf("Message handler complete. - counter: %d\n", counter);    // counter is accurate
            return;
        }

        // Caution: re-submit should be last operation
        if (server_mode) { 
            std::cout << __func__ << ": clients left: " << clients << std::endl;
            if (msgSize == -EPIPE) tcpSocket.RemoveSocket(req->_socketfd);

            // mark completion
            if (--clients <= 0) {
                // clients = tcpSocket.Broadcast(rwHandler, data, len+1);
                clients = tcpSocket.Broadcast(rwHandler, &m_datagrams[0]);
                usleep(1000000);
            }
        } else { 
            memset(data, 0, sizeof(data));
            tcpSocket.AsyncReceive(rwHandler, rwReq->_index); 
        }
        printf("Message handler complete. - counter: %d\n", counter);    // Important: counter stays 0
    };

    // acceptor
    iouring::MessageHandler acceptHandler = [&](iouring::Request *req, int socketfd) -> void {
        iouring::AcceptRequest *acceptReq = static_cast<iouring::AcceptRequest*>(req);
        std::cout << "AcceptHandler: received client connect from: " 
                  << inet_ntoa(acceptReq->_addr.sin_addr) << ":" 
                  << ntohs(acceptReq->_addr.sin_port) 
                  << std::endl;
        int clients = tcpSocket.AddSocket(socketfd);
        strncpy((char*)m_datagrams[0].msg_iov->iov_base, data, len);
        m_datagrams[0].msg_iov->iov_len = len;
        if (clients == 1) tcpSocket.Broadcast(rwHandler, &m_datagrams[0]);		// trigger write for first client
        tcpSocket.AsyncAccept(req->_callback);
    };

    if (server_mode) {
        tcpSocket.SetupListener(acceptHandler);
    } else {
        tcpSocket.Connect();
        snprintf(action, sizeof(action), "Received");
        tcpSocket.AsyncReceiveAll(rwHandler);
    }

    _ioring.Run();
    printf("%s: Done.\n", __func__);

    return 0;
}


