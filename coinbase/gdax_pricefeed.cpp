// TODO:
// 1. Fix formatting in both boost example as well as yours
// 2. Take command line argument for symbol to be subscribed
// 3. No message without type
// 4. Write another set of function to parse data using simple string parsing
//    [look at Bitfinex codebase, fastParseWs()]
//    NOTE: Only parse, L2Update (book), Trade/Ticker, Snapshot
//    - Try to receive snapshop data only once. (do it on last)

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <thread>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace rapidjson;

//------------------------------------------------------------------------------

inline void parseSnapshot(const char *json) {
        Document document;
        document.Parse(json);
        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        std::cout << "Type : " << document["type"].GetString() << std::endl;
        std::cout << "product Id : " << document["product_id"].GetString() << std::endl;
        //Gets The Value Of Asks
        auto asks = document["asks"].GetArray();
        std::cout << "Asks : [" << std::endl;
        for (SizeType i = 0; i < asks.Size(); i++)
        {
            auto &arrVal = asks[i];
            arrVal.Accept(writer);
            std::cout << sb.GetString() << std::endl;
            sb.Clear();
            writer.Reset(sb);
        }
        std::cout << "]" << std::endl;
        
        auto bids = document["bids"].GetArray();
        std::cout << "bids : [" << std::endl;
        for (SizeType i = 0; i < bids.Size(); i++)
        {
            auto &arrVal = bids[i];
            arrVal.Accept(writer);
            std::cout << sb.GetString() << std::endl;
            sb.Clear();
            writer.Reset(sb);
        }
        std::cout << "]" << std::endl;
}

inline void parseSubscriptions(const std::string& json) {
        Document document;
        document.Parse(json.c_str());
        std::cout << "type: " << document["type"].GetString() << std::endl;
        auto var = document["channels"].GetArray();
        std::cout << "channels: [";
        for (int idx=0; idx<var.Size(); ++idx) {
            auto &arrVal = var[idx];
            if (arrVal.IsString()) {
                std::cout << arrVal.GetString() << " ";
            } 
            else if (arrVal.IsObject()) {
                std::cout << "  name: " << arrVal["name"].GetString() << std::endl;
                auto jsonArr = arrVal["product_ids"].GetArray();
                std::cout << "  product_ids:[ ";
                for (int x=0; x<jsonArr.Size(); ++x) {
                    std::cout << jsonArr[x].GetString() << " ";
                }
                std::cout << "]" << std::endl;
            }
        }
        std::cout << "]" << std::endl;
}

inline void parseL2update(const std::string& json) {
        Document document;
        document.Parse(json.c_str());
        std::cout << "type: " << document["type"].GetString() << std::endl;
        std::cout << "product_id: " << document["product_id"].GetString() << std::endl;
        auto var = document["changes"].GetArray();
        std::cout << "changes: [";
        for (int idx=0; idx<var.Size(); ++idx) {
            auto &arrVal = var[idx];
            if (arrVal.IsArray()) {
                auto jsonArr = arrVal.GetArray();
                for (int x=0; x<jsonArr.Size(); ++x) {
                    std::cout << jsonArr[x].GetString() << " ";
                }
            }
        }
        std::cout << "]" << std::endl;
        std::cout << "time: " << document["time"].GetString() << std::endl;
}

inline void parseHeartbeat(const char *json) {
        Document document;
        document.Parse(json);
        std::cout << "type: " << document["type"].GetString() << std::endl;
        std::cout << "last_trade_id: " << document["last_trade_id"].GetInt64() << std::endl;
        std::cout << "product_id: " << document["product_id"].GetString() << std::endl;
        std::cout << "sequence: " << document["sequence"].GetInt64() << std::endl;
        std::cout << "time: " << document["time"].GetString() << std::endl;
        
}

inline void parseTicker(const char *json) {
        Document document;
        document.Parse(json);
        std::cout << "type: " << document["type"].GetString() << std::endl;
        std::cout << "sequence: " << document["sequence"].GetInt64() << std::endl;
        std::cout << "product_id: " << document["product_id"].GetString() << std::endl;
        std::cout << "price: " << document["price"].GetString() << std::endl;
        std::cout << "open_24h: " << document["open_24h"].GetString() << std::endl;
        std::cout << "volume_24h: " << document["volume_24h"].GetString() << std::endl;
        std::cout << "low_24h: " << document["low_24h"].GetString() << std::endl;
        std::cout << "high_24h: " << document["high_24h"].GetString() << std::endl;
        std::cout << "volume_30d: " << document["volume_30d"].GetString() << std::endl;
        std::cout << "best_bid: " << document["best_bid"].GetString() << std::endl;
        std::cout << "best_ask: " << document["best_ask"].GetString() << std::endl;
        std::cout << "side: " << document["side"].GetString() << std::endl;
        std::cout << "time: " << document["time"].GetString() << std::endl;
        std::cout << "trade_id: " << document["trade_id"].GetInt64() << std::endl;
        std::cout << "last_size: " << document["last_size"].GetString() << std::endl;
        
}
// Report a failure
void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

// Sends a WebSocket message and prints the response
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    websocket::stream<
    beast::ssl_stream<beast::tcp_stream>> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::string text_;
    unsigned msg_count_ = 0;

public:
    // Resolver and socket require an io_context
    explicit session(net::io_context& ioc, ssl::context& ctx)
        : resolver_(net::make_strand(ioc))
        , ws_(net::make_strand(ioc), ctx) {
    }

    // Start the asynchronous operation
    void run( char const* host, char const* port, char const* text) {
        std::cout << "Listener:" << __func__ << std::endl;
        // Save these for later
        host_ = host;
        text_ = text;

        // Look up the domain name
        resolver_.async_resolve(
            host,
            port,
            beast::bind_front_handler(
                &session::on_resolve,
                shared_from_this()));
    }

    void
    on_resolve(
        beast::error_code ec,
        tcp::resolver::results_type results)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "resolve");

        // Set a timeout on the operation
        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(ws_).async_connect(
            results,
            beast::bind_front_handler(
                &session::on_connect,
                shared_from_this()));
    }

    void
    on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "connect");

        // Set a timeout on the operation
        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(
                ws_.next_layer().native_handle(),
                host_.c_str()))
        {
            ec = beast::error_code(static_cast<int>(::ERR_get_error()),
                net::error::get_ssl_category());
            return fail(ec, "connect");
        }

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host_ += ':' + std::to_string(ep.port());
        
        // Perform the SSL handshake
        ws_.next_layer().async_handshake(
            ssl::stream_base::client,
            beast::bind_front_handler(
                &session::on_ssl_handshake,
                shared_from_this()));
    }

    void
    on_ssl_handshake(beast::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "ssl_handshake");

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(ws_).expires_never();

        // Set suggested timeout settings for the websocket
        ws_.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket_client_ssl");
            }));

        // Perform the websocket handshake
        ws_.async_handshake(host_, "/",
            beast::bind_front_handler(
                &session::on_handshake,
                shared_from_this()));
    }

    void
    on_handshake(beast::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "handshake");

        //// Read a message into our buffer
        //ws_.async_read(
        //    buffer_,
        //    beast::bind_front_handler(
        //        &session::on_read,
        //        shared_from_this()));

        // Send the message
        ws_.async_write(
            net::buffer(text_),
            beast::bind_front_handler(
                &session::on_write,
                shared_from_this()));
    }

    void
    on_write(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "write");

        // Read a message into our buffer
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &session::on_read,
                shared_from_this()));
    }

    std::string unsubscribe(std::string exchange, std::string symbol, std::string level) {
        std::ostringstream oss;
        oss << "{"
            << R"("type":"unsubscribe")"
            << R"(,"exchange":")" << exchange << R"(")"
            << R"(,"symbol":")" << symbol << R"(")"
            << R"(,"level":")" << level << R"(")"
            << "}";
	return oss.str();
    }

    void
    on_read(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        //std::cout << __func__ << ": bytes: " << bytes_transferred << std::endl;
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        // The make_printable() function helps print a ConstBufferSequence
        std::ostringstream oss;
        oss << beast::make_printable(buffer_.data());
        std::cout << __func__ << "-" << msg_count_ + 1 << ": " << oss.str() << std::endl;
        Document doc;
        doc.Parse(oss.str().c_str());
        std::string type = doc["type"].GetString();
        if(strcmp(type.c_str(), "subscriptions") == 0) {
            parseSubscriptions(oss.str().c_str());
    } else if(strcmp(type.c_str(), "l2update") == 0) {
            parseL2update(oss.str().c_str());
    } else if(strcmp(type.c_str(), "ticker") == 0) {
            parseTicker(oss.str().c_str());
    } else if(strcmp(type.c_str(), "heartbeat") == 0) {
            parseHeartbeat(oss.str().c_str());
    } else if(strcmp(type.c_str(), "snapshot") == 0) {
            parseSnapshot(oss.str().c_str());
    }

        // Clear the buffer
        buffer_.consume(buffer_.size());

        if (++msg_count_ == 50) {
            // Send the message
            ws_.async_write(
                net::buffer(this->unsubscribe("NBINE", "BTCUSDT", "L2|L1")),
                beast::bind_front_handler(
                    &session::on_write,
                    shared_from_this()));
	} else if (msg_count_ == 80) {
            ws_.async_write(
                net::buffer(text_),
                beast::bind_front_handler(
                    &session::on_write,
                    shared_from_this()));
	} else {
            // Read a message into our buffer
            sleep(1);
            ws_.async_read(
                buffer_,
                beast::bind_front_handler(
                    &session::on_read,
                    shared_from_this()));
	}
	/*
        if (++msg_count_ == 15) {
            // Close the WebSocket connection
            ws_.async_close(websocket::close_code::normal,
                beast::bind_front_handler(
                    &session::on_close,
                    shared_from_this()));
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Send the message
            ws_.async_write(
                net::buffer(text_),
                beast::bind_front_handler(
                    &session::on_write,
                    shared_from_this()));
        }
	*/
    }

    void
    on_close(beast::error_code ec)
    {
        std::cout << "Listener:" << __func__ << std::endl;
        if(ec)
            return fail(ec, "close");

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence
        std::cout << __func__ << std::endl;
    }
};

//------------------------------------------------------------------------------

int main()
{
    // Check command line arguments.
    auto const host = "ws-feed.exchange.coinbase.com";
    auto const port = "443";
    auto const text = "{\"type\":\"subscribe\",\"product_ids\":[\"ETH-USD\",\"ETH-EUR\"],\"channels\":[\"level2\",\"heartbeat\",{\"name\":\"ticker\",\"product_ids\":[\"ETH-BTC\",\"ETH-USD\"]}]}";

    // The io_context is required for all I/O
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12_client};

    // This holds the root certificate used for verification
    //load_root_certificates(ctx);

    // Launch the asynchronous operation
    std::make_shared<session>(ioc, ctx)->run(host, port, text);

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();

    return EXIT_SUCCESS;
}
