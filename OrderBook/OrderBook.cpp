#include "Include/OrderBook.hpp"
#include <boost/asio.hpp>

int main()
{
    boost::asio::io_context ctx;
    float askPrice = 33789.70;
    float bidPrice = 33659.50;
    float askAmount = 8.50;
    float bidAmount = 7.75;
    OrderBook o1(52, 120, 2, 16, ctx);

    for(int i = 0; i <= 20; i++)
    {
        if(i%2 == 0)
        {
            o1.AddData(askPrice, askAmount, true);
            askPrice -= 10;
            askAmount -= 0.75;
        }
        else
        {
            o1.AddData(bidPrice, bidAmount, false);
            bidPrice -= 10;
            bidAmount -= 0.75;
            o1.AddData(bidPrice, bidAmount, false);
        }
    }
    o1.RunOrderBook();
    ctx.run();
}