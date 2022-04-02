#include "Include/OrderBook.hpp"

int main()
{
    float askPrice = 33789.70;
    float bidPrice = 33659.50;
    float askAmount = 25.70;
    float bidAmount = 25.50;
    OrderBook o1;
    for(int i = 0; i < 45; i++)
    {
        if(i%2 == 0)
        {
            o1.AddData(askPrice, askAmount, true);
            askPrice -=100;
            askAmount -= 0.75;
        }
        else
        {
            o1.AddData(bidPrice, bidAmount, false);
            bidPrice -= 100;
            bidAmount -= 0.75;
        }
    }
    getch();
}
