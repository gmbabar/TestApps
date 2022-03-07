#include <ncurses.h>
#include <map>

typedef std::map<int, int, std::greater<int>> greaterMap;
typedef std::map<int, int> nMap;

class OrderBook{
public:
    explicit OrderBook(int height, int width, int yPos, int xPos) :  height(height), width(width), yPos(yPos), xPos(xPos) {}

    void Border()
    {
        int maxX = getmaxx(win)-2;
        int maxY = getmaxy(win);

        start_color();

        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);

        char c;
        int seprator = width/3;
        box(win, 0,0);

        for (int i = 4; i < height; i+=2)
        {
            mvwhline(win, i, 1, ACS_HLINE, maxX);
        }
        
        mvwvline(win, 1, seprator, ACS_VLINE, maxY);
        mvwvline(win, 1, seprator*2, ACS_VLINE, maxY);

        int title = seprator/2;
        wattron(win, COLOR_PAIR(1));
        mvwprintw(win, 2, title, "BID");
        wattroff(win, COLOR_PAIR(1));

        wattron(win, COLOR_PAIR(2));
        mvwprintw(win, 2, title+seprator, "Price");
        wattroff(win, COLOR_PAIR(2));
        
        wattron(win, COLOR_PAIR(3));
        mvwprintw(win, 2, title+2*seprator, "ASK");        
        wattroff(win, COLOR_PAIR(3));

        attron(COLOR_PAIR(4));
        mvprintw(height+2, title+seprator, "Press \'x\' And Then Press Enter To Exit");
        attroff(COLOR_PAIR(4));
        move(height+3, 0);

        refresh();
        wrefresh(win);
        while (c != 'x')
        {
            c = getch();
        }
        
        getch();
    }

    void AddData(float price, float quantity, bool ask)
    {
        int seprator = width/3;
        int title = seprator/2;
        if(ask)
        {
            int numberOfRowLevel = 1;
            asks.emplace(price, quantity);
            for(auto itr: asks)
            {
                mvwprintw(win, center-numberOfRowLevel, title+seprator, "%f", itr.first);
                mvwprintw(win, center-numberOfRowLevel, title+2*seprator, "%f", itr.second);
                numberOfRowLevel+=2;
            }
        }
        else
        {
            int numberOfRowLevel = 0;
            bids.emplace(price, quantity);
            for(auto itr: bids)
            {
                mvwprintw(win, center+numberOfRowLevel, title+seprator, "%f", itr.first);
                mvwprintw(win, center+numberOfRowLevel, title, "%f", itr.second);
                numberOfRowLevel+=2;
            }
        }
        refresh();
        wrefresh(win);
    }

    ~OrderBook(){}
private:
    nMap asks;
    greaterMap bids;
    int height;
    int width;
    int center = height/2;
    int yPos;
    int xPos;
    WINDOW *win = newwin(height, width, yPos, xPos);
};
