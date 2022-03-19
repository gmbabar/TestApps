#include "Include/OrderBook.hpp"
#include <boost/asio.hpp>

int main()
{
    boost::asio::io_context ctx;
    float askPrice = 33789.70;
    float bidPrice = 33659.50;
    float askAmount = 3.50;
    float bidAmount = 3.50;
    OrderBook o1(ctx);
    o1.RecieveFromSocket();
    ctx.run();
}