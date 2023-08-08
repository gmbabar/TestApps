//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class session
{
public:
        session(boost::asio::io_service& io_service, boost::asio::ssl::context& context)
    : socket_(io_service, context)
    {
    }

    ssl_socket::lowest_layer_type& socket()
    {
        return socket_.lowest_layer();
    }

    void start()
    {
        socket_.async_handshake(boost::asio::ssl::stream_base::server,
                boost::bind(&session::handle_handshake, this,
                    boost::asio::placeholders::error));
    }

    void handle_handshake(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                    boost::bind(&session::handle_read, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

    void handle_read(const boost::system::error_code& error,
            size_t bytes_transferred)
    {
        if (!error)
        {
            auto type = this->parseResponse(data_);
            std::string msg;
            if(type == 'A') {
                    msg = "8=FIX.4.2|9=172|35=A|34=1|49=Coinbase|52=20230808-14:46:30.660|56=c636bc5e6a36f1089f90e1c05ccc4d18|96=vqu3SkWPCuGkH2vo+TNtaCrv4hclSu9KBJdZd9UlJ+M=|98=0|108=30|141=Y|554=ncz86pr00bh|8013=Y|10=013|";
                    std::replace(msg.begin(), msg.end(), '|', '\x01');
                    boost::asio::async_write(socket_, boost::asio::buffer(msg), boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
                    std::cout << "Sending back: " << msg << std::endl;
                }
                else if (type == '0') {
                    msg = "8=FIX.4.2|9=58|35=0|49=Coinbase|56=c636bc5e6a36f1089f90e1c05ccc4d18|34=4|52=20190605-12:19:52.060|10=165|";
                    std::replace(msg.begin(), msg.end(), '|', '\x01');
                    boost::asio::async_write(socket_, boost::asio::buffer(msg), boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
                    std::cout << "Sending back: " << msg << std::endl;
                }
                // memset(buffer,0,sizeof(buffer));
                if(++m_sentCount == 15) {
                    exit(EXIT_SUCCESS);
                }
        }
        else
        {
        //     delete this;
        }
    }

    char parseResponse(const std::string &msg) {
        std::cout << "[RECEIVED DATA] " << msg << std::endl;
        auto key = msg.find("|35");
        if(key == std::string::npos) {
            key = msg.find('\x01'+"35");
            if (key == std::string::npos) {
                return '\0';
            }
        }
        const auto init = msg.find('=', key+1);
        if(init == std::string::npos) {
            return '\0';
        }

        const auto result = msg[init+1];
        std::cout << "MSG_TYPE: " << result << std::endl;
        return result;
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                    boost::bind(&session::handle_read, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

private:
    ssl_socket socket_;
    int m_sentCount = 0;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_service& io_service, unsigned short port, std::string host)
        : io_service_(io_service),
            acceptor_(io_service,
                    boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(host), port)),
            context_(boost::asio::ssl::context::sslv23)
    {
        // context_.set_options(
        //         boost::asio::ssl::context::default_workarounds
        //         | boost::asio::ssl::context::no_sslv2
        //         | boost::asio::ssl::context::single_dh_use);
        // context_.set_password_callback(boost::bind(&server::get_password, this));
        // context_.use_certificate_chain_file("server.pem");
        // context_.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
        // context_.use_tmp_dh_file("dh512.pem");

        session* new_session = new session(io_service_, context_);
        acceptor_.async_accept(new_session->socket(),
                boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
    }

    std::string get_password() const
    {
        return "test";
    }

    void handle_accept(session* new_session,
            const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
            new_session = new session(io_service_, context_);
            acceptor_.async_accept(new_session->socket(),
                    boost::bind(&server::handle_accept, this, new_session,
                        boost::asio::placeholders::error));
        }
        else
        {
            delete new_session;
        }
    }

private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ssl::context context_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        using namespace std; // For atoi.
        server s(io_service, atoi(argv[1]), "127.0.0.1");

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

.vscode/settings.json Bittrex/pricefeed Bittrex/pricefeed.cpp TestingFolder/Performance Test TestingFolder/Performance Test Without Native TestingFolder/Performance Test.cpp TestingFolder/PerformanceTest TestingFolder/TestScript.sh TestingFolder/process.sh benchmark/march_client benchmark/march_parser benchmark/march_server bitfinex/include/test bitfinex/include/test.cpp boost/asio/input/input boost/asio/input/input2 boost/lockfree/multiproducer/Makefile boost/lockfree/multiproducer/first boost/lockfree/multiproducer/first.cpp boost/lockfree/multiproducer/second boost/lockfree/multiproducer/second.cpp coinbase/Fix Api/bin/fix_api coinbase/gateway/MakeNewOrders.sh coinbase/gateway/coinbase_get_order coinbase/gateway/coinbase_get_orders gateway/.gitignore gateway/Makefile gateway/binance_balances.cpp gateway/coinbase_get_balance.cpp gemini/gateway/Makefile gemini/gateway/gemini_formatter gemini/gateway/gemini_rest_cancel gemini/gateway/gemini_rest_order_status gemini/gateway/gemini_rest_order_status.cpp gemini/gateway/gemini_rest_symbols gemini/gateway/gemini_websocket gemini/gateway/gemini_websocket.cpp
