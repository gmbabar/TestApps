//
// receiver.cpp
// ~~~~~~~~~~~~
//
// https://www.boost.org/doc/libs/1_69_0/doc/html/boost_asio/example/cpp11/multicast/receiver.cpp
//
// Input: multicast_address, multicast_port, interface_address

#include <array>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

short multicast_port = 30001;

class receiver {
public:
  receiver(boost::asio::io_context& io_context,
      const boost::asio::ip::address& listen_address,
      const boost::asio::ip::address& multicast_address)
    : socket_(io_context) {
    // Create the socket so that multiple may be bound to the same address.
    boost::asio::ip::udp::endpoint listen_endpoint(
        boost::asio::ip::address_v4::any(), multicast_port);
    socket_.open(listen_endpoint.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    // socket_.set_option(boost::asio::ip::multicast::enable_loopback(true));
    socket_.bind(listen_endpoint);
    // Join the multicast group.
    socket_.set_option(
        boost::asio::ip::multicast::join_group(multicast_address));

    do_receive();
  }

private:
  void do_receive() {
      socket_.async_receive_from(
          boost::asio::buffer(data_), sender_endpoint_,
    //socket_.async_receive(
    //    boost::asio::buffer(data_),
        [this](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            std::cout.write(data_.data(), length);
            std::cout << std::endl;
            do_receive();
          }
        });
  }

  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint sender_endpoint_;
  std::array<char, 1024> data_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 4) {
      std::cerr << "Usage: receiver <listen_address> <multicast_address>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 239.255.0.1 33001\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 ff31::8000:1234 3300\n";
      return 1;
    }

    multicast_port = atoi(argv[3]);

    boost::asio::io_context io_context;
    receiver r(io_context,
        boost::asio::ip::make_address(argv[1]),
        boost::asio::ip::make_address(argv[2]));
    io_context.run();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

