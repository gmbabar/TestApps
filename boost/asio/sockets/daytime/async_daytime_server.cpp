#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::io_context;
using boost::asio::buffer;
using boost::asio::async_write;
using boost::asio::placeholders::error;
using boost::asio::placeholders::bytes_transferred;

struct tcp_connection {
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(io_context& io_context) { return pointer(new tcp_connection(io_context)); }

    tcp::socket& socket() { return socket_; }

    void start() {
	time_t now = time(0);
        message_ = ctime( &now );

        async_write( socket_, buffer(message_), boost::bind(&tcp_connection::handle_write, this, error, bytes_transferred) );
    }

private:
    tcp_connection(io_context& io_context) : socket_(io_context) { }

    void handle_write(const boost::system::error_code& err, size_t written) { }

    tcp::socket socket_;
    std::string message_;
};

struct tcp_server {
    tcp_server(io_context& io_context) : acceptor_(io_context, tcp::endpoint(tcp::v4(), 13)) {
        start_accept();
    }

private:
    void start_accept() {
        tcp_connection::pointer new_conn = tcp_connection::create(acceptor_.get_executor().context());

        acceptor_.async_accept(new_conn->socket(), boost::bind(&tcp_server::handle_accept, this, new_conn, error) );
    }

    void handle_accept(tcp_connection::pointer new_conn, const boost::system::error_code& err) {
        if (!err) {
            new_conn->start();
        }

        start_accept();
    }

    tcp::acceptor acceptor_;
};

int main() {
    try {
        io_context io_context;
        tcp_server server(io_context);
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}


