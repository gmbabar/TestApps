#include "HeaderFile/order-book.hpp"

int main()
{
    initscr();
    OrderBook o1(36, 72, 2, 16);
    o1.Border();
    endwin();
}