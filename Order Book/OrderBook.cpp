#include "Include/OrderBook.hpp"

int main()
{
    initscr();
    float askPrice = 33789.70;
    float bidPrice = 33659.50;
    float askAmount = 4.50;
    float bidAmount = 4.00;
    OrderBook o1(36, 72, 2, 16);

    for(int i = 0; i <= 10; i++)
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

        o1.AddData(33660.70, 2.5, false);
        o1.AddData(33799.70, 2.5, true);
        o1.AddData(33809.70, 2.5, true);
        o1.AddData(33599.70, 2.5, false);
        o1.AddData(33589.70, 2.5, false);
        o1.AddData(33579.70, 2.5, false);

        
    }
    o1.Run();
    endwin();
}