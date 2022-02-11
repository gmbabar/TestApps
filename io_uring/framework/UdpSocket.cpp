// UdpSocket.cpp 

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
#include "UdpSocket.hpp"

// ----------------------------------------------------------------------------
// Example Multicast Receiver Code.
int main(int argc, char *argv[]) {
	char interface_ip[] = "127.0.0.1";
	char multicast_ip[] = "239.0.0.1";
	unsigned short mcast_sub_port = 12345;
	unsigned short mcast_pub_port = 23456;
	const int queue_size = 64;

    #define BUFFSIZE 128
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
        m_datagrams[idx].msg_iov	= &(m_recvbuff[idx].m_iovec);
        m_datagrams[idx].msg_iovlen   = 1;
        m_datagrams[idx].msg_name	= NULL;
        m_datagrams[idx].msg_namelen  = 0;
    }

    iouring::IoUring ioUring;
    iouring::UdpSocket udpRecvSocket(ioUring, mcast_sub_port, 
                                     m_datagrams, queue_size);
    udpRecvSocket.SubscribeMulticastGroup(multicast_ip, interface_ip);

    iouring::UdpSocket udpSendSocket(ioUring, multicast_ip, mcast_pub_port, 
                                     m_datagrams, queue_size);
    udpSendSocket.SetMulticastInterface(interface_ip);
    udpSendSocket.SetMulticastTtl();
    udpSendSocket.Connect();

    iouring::MessageHandler recvMessageHandler;
	iouring::MessageHandler sendMessageHandler = [&](iouring::Request *req, int msgSize) -> void {
        iouring::RwMsgRequest *rwReq = static_cast<iouring::RwMsgRequest*>(req);
        udpRecvSocket.AsyncReceive(recvMessageHandler, rwReq->_index); 
    };

    long int divide = std::chrono::system_clock::period::den / 1000000;
    long long total_duration = 0, counter=0, start_time, end_time;
	recvMessageHandler = [&](iouring::Request *req, int msgSize) -> void {
        iouring::RwMsgRequest *rwReq = static_cast<iouring::RwMsgRequest*>(req);
        char *msgData = static_cast<char*>(rwReq->_msghdr->msg_iov->iov_base);
        // Duration.
        end_time = static_cast<long int>(std::chrono::system_clock::now().time_since_epoch().count() / divide);
        start_time = strtoll(msgData, NULL, 10);
        total_duration += (end_time - start_time);
        if (++counter % 1000 == 0) {
            std::cout << "Average duration: " << static_cast<int>(total_duration / counter) 
                      << " microseconds " << std::endl;
            counter = 0;
            total_duration = 0;
        }
        udpSendSocket.AsyncSend(sendMessageHandler, rwReq->_index); 
	};

    udpRecvSocket.SetReuseAddress();
    udpRecvSocket.Bind();

    udpRecvSocket.AsyncReceiveAll(recvMessageHandler);

    ioUring.Run();

}


// // ----------------------------------------------------------------------------
// // Sample Udp sender/receiver code
// int main(int argc, char *argv[]) {
//     bool sender_mode = true;     // default

//     if (argc == 2 && !strcmp(argv[1], "-r")) {
//         sender_mode = false;
//     } else if ( argc != 1 && strcmp(argv[1], "-s") ) {
//         printf("\nInvalid arguments.\n");
//         printf("%s [-r|-s] \n", argv[0]);
//         return 0;
//     }

//     iouring::IoUring _ioring;
//     iouring::UdpSocket udpSocket(_ioring, 23456);
//     char action[16] = "Sent";
//     unsigned counter = 0;

//     // callback
//     iouring::MessageHandler messageHandler = [&](iouring::Request *req, int msgSize) -> void {
//         iouring::RwMsgRequest *rwReq = static_cast<iouring::RwMsgRequest*>(req);
//         char *msgData = static_cast<char*>(rwReq->_iovec.iov_base);
//         printf("%s %d bytes: %s - counter: %d \n", action, msgSize, msgData, ++counter);
//         if (counter == 10) {
//             _ioring.Shutdown();
//             printf("Message handler complete. - counter: %d\n", counter);    // counter is accurate
//             return;
//         }

//         // Caution: re-submit should be last operation
//         if (sender_mode) { 
//             udpSocket.AsyncSend(messageHandler, msgData, msgSize); 
//             usleep(100000);
//         } else { 
//             udpSocket.AsyncReceive(messageHandler, msgData, msgSize); 
//         }
//         printf("Message handler complete. - counter: %d\n", counter);    // Important: counter stays 0
//     };

//     char data[128];
//     size_t len = sprintf(data, "Test data being passed through udp socket.");

//     if (sender_mode) {
//         udpSocket.Connect();        // NOTE: connect fails if port is not bound by any socket and iouring doesn't support sendto()
//         udpSocket.AsyncSend(messageHandler, data, len+1);
//     } else {
//         snprintf(action, sizeof(action), "Received");
//         udpSocket.SetReuseAddress();
//         udpSocket.Bind();
//         udpSocket.AsyncReceive(messageHandler, data, sizeof(data));
//     }

//     _ioring.Run();
//     printf("%s: Done.\n", __func__);

//     return 0;
// }

