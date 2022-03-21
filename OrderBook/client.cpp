#include <boost/asio.hpp>
#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace boost::asio;
using namespace boost::asio::ip;

struct PriceData{
    float price;
    float quantity;
    bool is_ask;
};


int main()
{
    float askPrice = 33789.70;
    float bidPrice = 33659.50;
    float askAmount = 25.50;
    float bidAmount = 25.50;
    char buffer[128];

    io_context ctx;
    ip::udp::socket m_socket(ctx);
    udp::endpoint server(make_address("127.0.0.1"), 1234);
    PriceData p1;

    m_socket.open(udp::v4());

    for(int i = 0; i < 25; i++)
    {
        if(i%2 == 0)
        {
            p1.is_ask = true;
            p1.price = askPrice;
            p1.quantity = askAmount;
            sprintf(buffer, "%f, %f, %d", p1.price, p1.quantity, p1.is_ask);
            m_socket.send_to(boost::asio::buffer(buffer), server);
            askPrice -= 10;
            askAmount -= 0.75;
        }
        else
        {
            p1.is_ask = false;
            p1.price = bidPrice;
            p1.quantity = bidAmount;
            sprintf(buffer, "%f, %f, %d", p1.price, p1.quantity, p1.is_ask);
            m_socket.send_to(boost::asio::buffer(buffer), server);
            bidPrice -= 10;
            bidAmount -= 0.75;
        }
        sleep(1);
    }
}