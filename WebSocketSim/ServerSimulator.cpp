#include <iostream>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace std;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace websocket = beast::websocket;


class Session{
public:

    Session(tcp::socket && socket) : ws_(std::move(socket)){}

    void Run()
    {
        net::dispatch(ws_.get_executor(), beast::bind_front_handler(&Session::OnRun, this));
    }

private:

    void OnRun()
    {
        ws_.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::server));
        
        ws_.async_accept(beast::bind_front_handler(&Session::OnAccept,
        this, boost::placeholders::_1));
    }

    void OnAccept(beast::error_code ec)
    {
        if(ec)
        {
            cerr << "Accept : " << ec.message() << endl;
            return;
        }
        // doRead();
    }

    void SetupRead()
    {
        ws_.async_read(buffer_, beast::bind_front_handler(&Session::ReadHandler,
        this, boost::placeholders::_1, boost::placeholders::_2));
    }

    void ReadHandler(beast::error_code &ec, size_t bytesRead)
    {
        boost::ignore_unused(bytesRead);

        if(ec == beast::websocket::error::closed)
        {
            cout << "Server Closed" << std::endl;
            return;
        }
        else if(ec)
        {
            cout << "Read : " << ec.message() << endl;
            return;
        }
        else
        {
            ws_.text(ws_.got_text());
            std::string s1(boost::asio::buffer_cast<const char*>(buffer_.data()));
            std::cout << s1 << std::endl;
            if(s1 == "Verify")
            {
                auto prepBuff = auth_.prepare(8);
                char *p = static_cast<char *>(prepBuff.data());
                memcpy(p, "true", 4);
                auth_.commit(4);
                ws_.async_write(auth_.data(), 
                beast::bind_front_handler(&Session::WriteHandler, this
                ,boost::placeholders::_1, boost::placeholders::_2));
            }
        }
    }

    void WriteHandler(beast::error_code ec, size_t readBytes)
    {
        if(ec)
        {
            cerr << "Write : " << ec.message() << endl;
            return; 
        }

        auth_.consume(auth_.size());
        SetupRead();
    }


    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
    beast::flat_buffer auth_;

};




class Acceptor{
public:
    Acceptor(net::io_context &ctx, tcp::endpoint endpoint) : ctx_(ctx), sock_(net::make_strand(ctx)),
    acceptor_(net::make_strand(ctx_))
    {
        beast::error_code ec;

        acceptor_.open(endpoint.protocol(), ec);
        if(ec)
        {
            cerr << "Open : " << ec.message() << endl;
            return;
        }

        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if(ec)
        {
            cerr << "Set_option : " << ec.message() << endl;
            return;
        }

        acceptor_.bind(endpoint, ec);
        if(ec)
        {
            cerr << "bind : " << ec.message() << endl;
            return;
        }

        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if(ec)
        {
            cerr << "Listen : " << ec.message() << endl;
            return;
        }
    }

    void Accept()
    {

    }
private:

    void DoAccept(beast::error_code ec)
    {
        acceptor_.async_accept(sock_, std::bind(&Acceptor::DoAccept, this, std::placeholders::_1));
        if(ec)
        {
            cerr << "Accept : " << ec.message() << endl;
        }
        else
        {

        }
    }

    net::io_context &ctx_;
    tcp::socket sock_;
    tcp::acceptor acceptor_;

};

int main()
{
    /*Your Code Here*/
    
    return 0;
}
