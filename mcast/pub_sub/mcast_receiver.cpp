/* Receiver/client multicast Datagram. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
// #include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string>
#include <iostream>
#include <csignal>
#include <chrono>

namespace
{
    volatile std::sig_atomic_t signal_status;
    struct sockaddr_in localSock;
    struct ip_mreq group;
    int sockfd;
    char databuf[1024];
    int datalen = sizeof(databuf);
}

// Wait for when ctrl-c (SIGINT) to stop receiver
void signal_handler(int signum)
{
    std::cout << "Caught signal: SIGINT " << std::endl;
    signal_status = signum;
    close(sockfd);
}

int main(int argc, char *argv[])
{
    // Register signal and signal handler
    signal(SIGINT, signal_handler); // val: 2 - ctrl+c

    std::string interface_ip = "127.0.0.1";
    std::string multicast_ip = "239.0.0.1";
    unsigned short multicast_port = 23456;

    std::string input;
    std::cout << "Enter interface address, enter to use default [" << interface_ip << "]: ";
    std::getline(std::cin, input);
    if (input.size() > 0)
    {
        interface_ip = input;
    }

    std::cout << "Enter multicast address, enter to use detault [" << multicast_ip << "]: ";
    std::getline(std::cin, input);
    if (input.size() > 0)
    {
        multicast_ip = input;
    }

    std::cout << "Enter multicast port, enter to use detault [" << multicast_port << "]: ";
    std::getline(std::cin, input);
    if (input.size() > 0)
    {
        multicast_port = static_cast<unsigned short>(std::atoi(input.c_str()));
    }

    /* Create a datagram socket on which to receive. */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Opening datagram socket error");
        exit(1);
    }
    std::cout << "Opening datagram socket....OK.\n";

    /* Enable SO_REUSEADDR to allow multiple instances of this */
    /* application to receive copies of the multicast datagrams. */
    {
        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
        {
            perror("Setting SO_REUSEADDR error");
            close(sockfd);
            exit(1);
        }
        std::cout << "Setting SO_REUSEADDR...OK.\n";
    }

    /* Bind to the proper port number with the IP address */
    /* specified as INADDR_ANY. */
    memset((char *)&localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(multicast_port);
    localSock.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *)&localSock, sizeof(localSock)))
    {
        perror("Binding datagram socket error");
        close(sockfd);
        exit(1);
    }
    std::cout << "Binding datagram socket...OK.\n";

    /* Join the multicast group [226.1.1.1] on the local [127.0.0.1] */
    /* interface. Note that this IP_ADD_MEMBERSHIP option must be */
    /* called for each local interface over which the multicast */
    /* datagrams are to be received. */
    group.imr_multiaddr.s_addr = inet_addr(multicast_ip.c_str());
    group.imr_interface.s_addr = inet_addr(interface_ip.c_str());
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
    {
        perror("Adding multicast group error");
        close(sockfd);
        exit(1);
    }
    std::cout << "Adding multicast group...OK.\n";

    std::cout << "Reading datagram message ... press ctrl+c to stop" << std::endl;
    /* Read from the socket. */
    int count = 0, total_data_size = 0;
    int bytes_read = 0;
    long int divide = std::chrono::system_clock::period::den / 1000000;
    long long total_duration = 0, start_time, end_time;
    while (signal_status == 0)
    {
        // NOTE: Both read() and recevfrom() do the same job
        if ((bytes_read = read(sockfd, databuf, datalen)) < 0)
        {
            if (errno == EBADF) break;  // It's closed by interrupt handler
            perror("Reading datagram message error");
            close(sockfd);
            exit(1);
        }
        //std::cout << "Received Msg: " << databuf << std::endl;

        // Duration.
        end_time = static_cast<long int>(std::chrono::system_clock::now().time_since_epoch().count() / divide);
        start_time = strtoll(databuf, NULL, 10);

        total_data_size += bytes_read;
        total_duration += (end_time - start_time);
        ++count;
if (count % 1000 == 0) {
    std::cout << "Average duration: " << static_cast<int>(total_duration / count) << " microseconds" << std::endl;
    //count = 0; total_duration = 0;
}
    }
    std::cout << "Last Msg: " << databuf << std::endl;
    std::cout << "Last start time: " << start_time << std::endl;
    std::cout << "Last end   time: " << end_time << std::endl;
    std::cout << "FINAL STATS:" << std::endl;
    std::cout << "Received " << total_data_size << " bytes in " << count
              << " messages from multicast server" << std::endl;
    std::cout << "Total duration: " << total_duration << " for " << count << " messages." << std::endl;
    std::cout << "Average duration: " << static_cast<int>(total_duration / count) << " microseconds" << std::endl;
    return 0;
}



