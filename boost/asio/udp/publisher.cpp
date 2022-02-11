
// https://www.boost.org/doc/libs/1_78_0/doc/html/boost_asio/example/cpp03/chat/posix_chat_client.cpp

#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

//using boost::asio::ip::udp;
using namespace boost::asio::ip;
namespace posix = boost::asio::posix;	// input stream / stdin

// TODO: Instead of defining global use lambda function
boost::asio::streambuf buffer(256);
boost::asio::io_service io_context;   // io_service is older
udp::socket m_udpSocket(io_context);
posix::stream_descriptor input_(io_context, ::dup(STDIN_FILENO));	// stdin
udp::endpoint remote_endpoint;

void handle_read_input(const boost::system::error_code& error, size_t length);

void inputRead() {
    // Read a line of input entered by the user.
    std::cout << "Enter string: " << std::endl;
    boost::asio::async_read_until(input_, buffer, '\n', handle_read_input);
}

void handle_send(const boost::system::error_code& error, std::size_t nbytes) {
    inputRead();
}

void handle_read_input(const boost::system::error_code& error, size_t length) {
    if (!error) {
      // Write the message to the server.
      std::ostringstream ss;
      ss << &buffer;
      std::string msg = ss.str().substr(0, length-1);	// strip-off \n
      std::cout << "Publishing: '" << msg << "' to " << remote_endpoint << std::endl;
      // target endpoint.
      m_udpSocket.async_send_to(boost::asio::buffer(msg), remote_endpoint, handle_send);
      // boost::asio::async_write(socket_, message, handle_write);
    } else {
      std::cout << "Error reading input" << std::endl;
      exit(1);
    }
}

inline udp::endpoint resolveUdp(
       boost::asio::io_context &anIos,
       std::string host, std::string port) {
    try {
        udp::resolver::query query(host, port);
        udp::resolver resolver(anIos);
        return *resolver.resolve(query); // throws boost::system::system_error
    }
    catch(const std::exception &se) {
        std::stringstream ss;
        ss << "Failed to resolve " << host << ":" << port << ", err: " << se.what();
        std::cerr << ss.str() << std::endl;
        throw std::invalid_argument(ss.str());
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
	std::cout << "Usage: " << std::endl;
	std::cout << "\t" << argv[0] << " <host> <port>" << std::endl;
	exit(0);
    }

    remote_endpoint = resolveUdp(io_context, argv[1], argv[2]);

    // Setup socket
    m_udpSocket.open(udp::v4());
    std::cout << "Created udp socket" << std::endl;

    // Post the first recv
    std::cout << "Using boost async publish." << std::endl;
    io_context.post(inputRead);

    std::cout << "Running context" << std::endl;
    io_context.run();

    std::cout << "Done." << std::endl;
}


