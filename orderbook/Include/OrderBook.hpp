#include <ncurses.h>
#include <map>

typedef std::map<float, float, std::greater<float>> bidMap;
typedef std::map<float, float> askMap;

class OrderBook{
public:
    OrderBook()
    {
        initscr();
        noecho();
        getmaxyx(stdscr, scrMaxY, scrMaxX);
        win_ = newwin(scrMaxY/1.25, scrMaxX/1.2, scrMaxY/10, scrMaxX/10);
        getmaxyx(win_, winMaxY, winMaxX);
        this->Border();
    }

    void AddData(float price, float quantity, bool ask)
    {
        if(quantity <= 0)
        {
            if(ask)
            {
                asks.erase(price);
            }
            else
            {
                bids.erase(price);
            }
        }
        else
        {
            if(ask)
            {
                asks[price] += quantity;
                PrintData(ask);
            }
            else
            {
                bids[price] += quantity;
                PrintData(ask);
            }
        }
    }

    ~OrderBook(){
        endwin();
    }
private:


    void Border()
    {
        float addition = (winMaxY-5);
        addition = addition/20;
        
        int seprator = winMaxX/3;
        int center = winMaxY/2;

        start_color();

        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        box(win_, 0,0);
        
        for(int i = 6; i < winMaxY; i+=2)
        {
            mvwhline(win_, i, 1, ACS_HLINE, winMaxX-2);
        }
        
        mvwvline(win_, 1, seprator, ACS_VLINE, winMaxY);
        mvwvline(win_, 1, seprator*2, ACS_VLINE, winMaxY);

        int title = seprator/2;
        wattron(win_, COLOR_PAIR(1));
        mvwprintw(win_, 3, title, "\"BID\"");
        wattroff(win_, COLOR_PAIR(1));

        wattron(win_, COLOR_PAIR(2));
        mvwprintw(win_, 3, title+seprator, "\"Price\"");
        wattroff(win_, COLOR_PAIR(2));
        
        wattron(win_, COLOR_PAIR(3));
        mvwprintw(win_, 3, title+2*seprator, "\"ASK\"");        
        wattroff(win_, COLOR_PAIR(3));

        move(winMaxY+3, 0);
        refresh();
        wrefresh(win_);
    }

    void PrintData(bool isAsk)
    {
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        
        int center = (winMaxY/2)+2;
        int seprator = winMaxX/3;
        int title = seprator/2;
        int numberOfRowLevel = 1;
        float addition = (winMaxY-6);
        addition = addition/20;

        if(isAsk)
        {
            for(auto itr: asks)
                {
                    if(center-numberOfRowLevel < (addition)*3)
                    {
                        numberOfRowLevel = winMaxY;
                    }

                    wattron(win_, COLOR_PAIR(3));
                    mvwprintw(win_, center-numberOfRowLevel, title+2*seprator, "%.2f", itr.second);
                    wattroff(win_, COLOR_PAIR(3));
                    
                    wattron(win_, COLOR_PAIR(2));
                    mvwprintw(win_, center-numberOfRowLevel, title+seprator, "%.2f", itr.first);
                    wattroff(win_, COLOR_PAIR(2));
                    numberOfRowLevel+=2;
                }
            numberOfRowLevel = 1;
        }
        else
        {
            for(auto itr: bids)
                {
                    if(numberOfRowLevel == center)
                    {
                        numberOfRowLevel = winMaxY;
                    }

                    wattron(win_, COLOR_PAIR(1));
                    mvwprintw(win_, center+numberOfRowLevel, title, "%.2f", itr.second);
                    wattroff(win_, COLOR_PAIR(1));

                    wattron(win_, COLOR_PAIR(2));
                    mvwprintw(win_, center+numberOfRowLevel, title+seprator, "%.2f", itr.first);
                    wattroff(win_, COLOR_PAIR(2));

                    numberOfRowLevel+=2;
                }
            numberOfRowLevel = 1;
        }
        refresh();
        wrefresh(win_);
    }
    
    askMap asks;
    bidMap bids;
    int scrMaxX, scrMaxY, winMaxX, winMaxY;
    WINDOW *win_;
};