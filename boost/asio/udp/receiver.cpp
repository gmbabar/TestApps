
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

//using boost::asio::ip::udp;
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

    // address.
    //const address listenAddr(address::from_string("127.0.0.1")); 	// boost::asio::ip::address
    //const udp::endpoint listenEp(listenAddr, portNum);
    // - OR -
    const udp::endpoint listenEp(udp::v4(), portNum);

    // Setup socket
    m_udpSocket.open(listenEp.protocol());
    m_udpSocket.set_option(udp::socket::reuse_address(true));
    m_udpSocket.bind(listenEp);

    std::cout << "Using udp unicast, listening on: " << listenEp << std::endl;

    // Post the first recv
    std::cout << "Using boost async recv." << std::endl;
    io_context.post(udpReceive);

    std::cout << "Running context" << std::endl;
    io_context.run();

    std::cout << "Done." << std::endl;
}

