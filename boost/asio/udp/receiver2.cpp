/***
 This boilerplat code will check which udp port is available to bind
***/


#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace boost::asio::ip;

// TODO: Instead of defining global use lambda function
boost::array<char, 256> message;
boost::asio::io_service io_context;   // io_service is older
udp::socket m_udpSocket(io_context);

void handleUdpReceived(const boost::system::error_code &ec, const std::size_t aBytesRcvd);

void udpReceive() {
    m_udpSocket.async_receive(boost::asio::buffer(message), handleUdpReceived);
    // OR
    //udp::endpoint remote_endpoint;
    //m_udpSocket.async_receive_from(boost::asio::buffer(message), remote_endpoint, handleUdpReceived);
}

void handleUdpReceived(const boost::system::error_code &ec, const std::size_t aBytesRcvd) {
    if(!ec) {
	std::cout << "Received: " << aBytesRcvd << ", data: " << message.data() << std::endl;
        udpReceive();
    } else {
	std::cout << "An error occured while receiving tick data: " << ec << std::endl;
    }
}

int main() {
    unsigned portNum = 23456;


    // Setup socket
    m_udpSocket.open(udp::v4());

    // Bind available port.
    unsigned endPortNum = 23458;
    while (portNum <= endPortNum) {
        try {
            m_udpSocket.bind(udp::endpoint(udp::v4(), portNum));
	    break;
        } catch (boost::system::system_error &e) {
            std::cout << "Failed to bind on port: " << portNum << std::endl;
        } catch (...) {
            std::cout << "Unkownn failure while binding on port: " << portNum << std::endl;
	    exit(1);
        }
        if( ++portNum == endPortNum) {
	    std::cout << "Couldn't find any available port ... existing" << std::endl;
	    exit(0);
	}
    }

    std::cout << "Using udp, listening on: " << portNum << std::endl;

    // Post the first recv
    std::cout << "Using boost async recv." << std::endl;
    io_context.post(udpReceive);

    std::cout << "Running context" << std::endl;
    io_context.run();

    std::cout << "Done." << std::endl;
}

