#include <ncurses.h>
#include <map>

typedef std::map<int, int, std::greater<int>> greaterMap;
typedef std::map<int, int> nMap;

class OrderBook{
public:
    explicit OrderBook(int height, int width, int yPos, int xPos) :  height_(height), width_(width), yPos_(yPos), xPos_(xPos) {}

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
        int seprator = width_/3;
        box(win, 0,0);

        for (int i = 4; i < height_; i++)
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
        mvprintw(height_+2, title+seprator, "Press \'x\' And Then Press Enter To Exit");
        attroff(COLOR_PAIR(4));
        move(height_+3, 0);

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
        int seprator = width_/3;
        int title = seprator/2;
        if(ask)
        {
            int numberOfRowLevel = 0;
            asks.emplace(price, quantity);
            for(auto itr: asks)
            {
                mvwprintw(win, center_-numberOfRowLevel, title+seprator, "%f", itr.first);
                mvwprintw(win, center_-numberOfRowLevel, title+2*seprator, "%f", itr.second);
                numberOfRowLevel+=2;
            }
        }
        else
        {
            int numberOfRowLevel = 0;
            bids.emplace(price, quantity);
            for(auto itr: bids)
            {
                mvwprintw(win, center_+numberOfRowLevel, title+seprator, "%f", itr.first);
                mvwprintw(win, center_+numberOfRowLevel, title, "%f", itr.second);
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
    int height_;
    int width_;
    int center_ = height_/2;
    int yPos_;
    int xPos_;
    WINDOW *win = newwin(height_, width_, yPos_, xPos_);
};
