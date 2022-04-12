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

#include <string>
#include <nebula/data/BufRange.hpp>
#include <nebula/data/HostPort.hpp>
#include <nebula/net/recvmmsg.hpp>

#include <boost/thread/thread.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;
    
#include <boost/asio.hpp>
namespace io = boost::asio;

#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

#include "iouring_msgrecv.hpp"
#include "iouring_msgsend.hpp"

// Return the interface name for the socket
std::string ifNameForSocket(int aSocket) {
    // Reference: https://stackoverflow.com/questions/848040/how-can-i-get-the-interface-name-index-associated-with-a-tcp-socket
    struct sockaddr_in addr;
    struct ifaddrs* ifaddr;
    struct ifaddrs* ifa;
    socklen_t addr_len;

    addr_len = sizeof (addr);
    getsockname(aSocket, (struct sockaddr*)&addr, &addr_len);
    getifaddrs(&ifaddr);

    // look which interface contains the wanted IP.
    // When found, ifa->ifa_name contains the name of the interface (eth0, eth1, ppp0...)
    std::string name;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr) {
            if (AF_INET == ifa->ifa_addr->sa_family) {
                struct sockaddr_in* inaddr = (struct sockaddr_in*)ifa->ifa_addr;
                if (inaddr->sin_addr.s_addr == addr.sin_addr.s_addr) {
                    if (ifa->ifa_name) {
                        name = ifa->ifa_name;
                    }
                }
            }
        }
    }
    freeifaddrs(ifaddr);
    return name;
}

std::string ipAddressForIf(const std::string &anIf) {
    // Reference: https://stackoverflow.com/questions/212528/get-the-ip-address-of-the-machine
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    std::string address;
    for(ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if(!ifa->ifa_addr) {
            continue;
        }
        if(ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if(strcmp(ifa->ifa_name, anIf.c_str()) == 0)
                address = std::string(&addressBuffer[0]);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            if(strcmp(ifa->ifa_name, anIf.c_str()) == 0)
                address = std::string(&addressBuffer[0]);
        } 
    }
    if (ifAddrStruct!=NULL) 
        freeifaddrs(ifAddrStruct);
    return address;
}

int main(int argc, char* argv[]) {
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "produce help message")
        ("force,f", "force sub/pub even if data detected already on publish multicast")
        ("sub,s", po::value<std::string>(), "Subscribe multicast.")
        ("pub,p", po::value<std::string>(), "Publish multicast.")
        ("int,i", po::value<std::string>(), "Publish interface ip address.")
        ("spin,b", "Use busy polling on socket to receive messages.")
        ;

    auto usage = [&]() {
        std::cerr << argv[0] << " -s 239.xxx.xxx.xxx:12345 -p 239.yyy.yyy.yyy:12345 -i 10.zzz.zzz.zzz" << std::endl;
        std::cerr << argv[0] << " -s 239.xxx.xxx.xxx:12345@edge0 -p 239.yyy.yyy.yyy:12345@eth2 -i 10.zzz.zzz.zzz" << std::endl;
    };

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help") 
            || !vm.count("pub")
            || !vm.count("sub")) {
            usage();
            return 1;
        }
    } catch (const po::error &ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        usage();
        return 1;
    }

    // Parse subscription details
    std::string subMcastStr = vm["sub"].as<std::string>();
    std::string subMcastInt;
    auto subAt = subMcastStr.find('@');
    if(subAt != std::string::npos) {
        subMcastInt = subMcastStr.substr(subAt + 1);
        subMcastStr = subMcastStr.substr(0, subAt);
    }
    nebula::data::HostPort subMcast(subMcastStr);

    // Parse prod details
    std::string pubMcastStr = vm["pub"].as<std::string>();
    std::string pubMcastInt;
    auto pubAt = pubMcastStr.find('@');
    if(pubAt != std::string::npos) {
        pubMcastInt = pubMcastStr.substr(pubAt + 1);
        pubMcastStr = pubMcastStr.substr(0, pubAt);
    }
    nebula::data::HostPort pubMcast(pubMcastStr);

    // Parse prod publish interface 
    std::string pubIntIP = vm.count("int") ? vm["int"].as<std::string>() : std::string();
    if(!pubIntIP.empty() && !pubMcastInt.empty()) {
        std::cerr << "Cannot specify both publishing interface ip and device name! pub_ip="
                  << pubIntIP << " pub_int=" << pubMcastInt << std::endl;
        return 1;
    }
    else if(pubIntIP.empty()) {
        pubIntIP = ipAddressForIf(pubMcastInt);
        if(pubIntIP.empty()) {
            std::cerr << "Failed to find ip address for publish interface " << pubMcastInt << '!' << std::endl;
            return 1;
        }
    }
    if(pubIntIP.empty()) {
        std::cerr << "No publish interface or IP specified!" << std::endl;
        return 1;
    }
        
    // Verify no data on the publish socket multicast
    if(!vm.count("force")) {
        // Setup subscribe socket
        auto testSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if(testSocket == -1) {
            std::cerr << "Failed to create test socket" << std::endl;
            return 1;
        }
        
        // Resuse addr
        {
            int enable = 1;
            if(setsockopt(testSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
                std::cerr << "setsockopt(SO_REUSEADDR) failed" << std::endl;
                return 1;
            }
        }
        
        // Subscribe to multicast group
        // Reference:
        // https://stackoverflow.com/questions/48624503/c-receiving-multicast-on-particular-interface
        // https://www.tldp.org/HOWTO/Multicast-HOWTO-6.html
        {
            ip_mreqn group;
            group.imr_multiaddr.s_addr = inet_addr(pubMcast.host.c_str());
            group.imr_address.s_addr = inet_addr(pubIntIP.c_str());
            //group.imr_address.s_addr = htonl(INADDR_ANY);
            if(!pubMcastInt.empty())
                group.imr_ifindex = if_nametoindex(pubMcastInt.c_str());
            else
                group.imr_ifindex = 0;
            if(setsockopt(testSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
                std::cerr << "Failed to add test subscription to multicast group!" << std::endl;
                close(testSocket);
                return 1;
            }
            std::cout << "Joining udp multicast: " << pubMcastStr << " interface=" << pubMcastInt << std::endl;
        }

        // Publish address
        struct sockaddr_in pubAddr;
        memset((char *) &pubAddr, 0, sizeof(pubAddr));
        pubAddr.sin_family = AF_INET;
        pubAddr.sin_addr.s_addr = inet_addr(pubMcast.host.c_str());
        pubAddr.sin_port = htons(pubMcast.portNum);

        // Bind to address
        bind(testSocket, (sockaddr*)&pubAddr, sizeof(pubAddr));

        // Verify no data on publish multicast/interface
        const long stopSecs = 20;
        std::cout << "Verifying no data on publish multicast " << pubMcast.host 
                  << " for " << stopSecs << " seconds... ";
        std::flush(std::cout);
        char buf[2048];
        auto stopTm = nebula::dtm::microsec_clock::local_time() + nebula::dtm::seconds(stopSecs);
        while(nebula::dtm::microsec_clock::local_time() < stopTm) {
            const auto c = recv(testSocket, &buf[0], sizeof(buf), MSG_DONTWAIT);
            if(c > 0) {
                std::cout << "FAILED! Found data on publish multicast!" << std::endl;
                close(testSocket);
                return 1;
            }
            usleep(1000);
        }
        std::cout << "Done" << std::endl;;
        close(testSocket);
    }
    
    // mcast publisher
    std::size_t queue_size = 16;
    iouring::MulticastPublisher publisher(pubIntIP, pubMcast.host, pubMcast.portNum, queue_size);

    // mcast publisher
    queue_size = 80;
    iouring::MulticastReceiver receiver(pubIntIP, subMcast.host, subMcast.portNum, queue_size);

    // Setup signal handlers for clean file writing on shutdown
    io::io_service ios;
    volatile bool running = true;
    auto signalHandler = [&](const boost::system::error_code &ec, int signum) {
        std::cerr << "Caught signal: " << signum << " ec=" << ec << std::endl;
        receiver.StopReceiver();
        running = false;
        ios.stop();
    };
    nebula::asio::io_service::work work(ios);
    boost::asio::signal_set signals(ios, SIGINT, SIGTERM);
    signals.async_wait(signalHandler);
    boost::thread bgThread(boost::bind(&io::io_service::run, &ios));
    
    // Receive data loop
    std::cout << "Starting sub-pub " << subMcast << " -> " << pubMcast << std::endl;
    boost::system::error_code ec;
    while(running) {
        iouring::MessageHandler messageHandler = [&](char *msgData, size_t msgSize) -> void {
            publisher.SendMessage(msgData, msgSize);
        };
        receiver.StartReceiver(messageHandler);
    }
        
    bgThread.join();
    return 0;
}
