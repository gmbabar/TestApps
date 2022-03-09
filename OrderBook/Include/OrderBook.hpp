#include <ncurses.h>
#include <map>

typedef std::map<float, float, std::greater<float>> greaterMap;
typedef std::map<float, float> nMap;

class OrderBook{
public:
    OrderBook(int heightOfBook, int widthOfBook, int yPosition, int xPosition) :  height(heightOfBook), width(widthOfBook), yPos(yPosition), xPos(xPosition) {
        initscr();
        win = newwin(height, width, yPos, xPos);
        Border();
    }

    void Border()
    {
        int seprator = width/3;
        int center = height/2;
        int maxX = getmaxx(win)-2;
        int maxY = getmaxy(win);

        start_color();

        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        box(win, 0,0);

        for (int i = 4; i < height; i+=2)
        {
            mvwhline(win, i, 1, ACS_HLINE, maxX);
        }
        
        mvwvline(win, 1, seprator, ACS_VLINE, maxY);
        mvwvline(win, 1, seprator*2, ACS_VLINE, maxY);

        int title = seprator/2;
        wattron(win, COLOR_PAIR(1));
        mvwprintw(win, 2, title, "\"BID\"");
        wattroff(win, COLOR_PAIR(1));

        wattron(win, COLOR_PAIR(2));
        mvwprintw(win, 2, title+seprator, "\"Price\"");
        wattroff(win, COLOR_PAIR(2));
        
        wattron(win, COLOR_PAIR(3));
        mvwprintw(win, 2, title+2*seprator, "\"ASK\"");        
        wattroff(win, COLOR_PAIR(3));

        attron(COLOR_PAIR(4));
        mvprintw(height+3, title+seprator, "\tPress \'q\' To Exit");
        attroff(COLOR_PAIR(4));
        move(height+3, 0);
    }

    void AddData(float price, float quantity, bool ask)
    {
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        int center = height/2;
        int seprator = width/3;
        int title = seprator/2;
        int numberOfRowLevel = 1;
        if(ask)
        {
            if(center-numberOfRowLevel < 4)
            {
                attron(COLOR_PAIR(4));
                mvprintw(height+5, seprator+title, "\t[Warning] Columns Filled The Text Will Not Appear After Specific Height");
                mvprintw(height+6, seprator+title, "\t[Hint] Increase Height Of OrderBook");
                attroff(COLOR_PAIR(4));
            }
            asks.emplace(price, quantity);
            for(auto itr: asks)
            {
                wattron(win, COLOR_PAIR(3));
                mvwprintw(win, center-numberOfRowLevel, title+2*seprator, "%.2f", itr.second);
                wattroff(win, COLOR_PAIR(3));
                
                wattron(win, COLOR_PAIR(2));
                mvwprintw(win, center-numberOfRowLevel, title+seprator, "%.2f", itr.first);
                wattroff(win, COLOR_PAIR(2));
                numberOfRowLevel+=2;

            }
        }
        else
        {
            bids.emplace(price, quantity);
            for(auto itr: bids)
            {
                if(numberOfRowLevel== center-3)
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(height+5, seprator+title, "\t[Warning] Columns Filled The Data Will Not Appear After Specific Height");
                    mvprintw(height+6, seprator+title, "\t[Hint] Increase Height Of OrderBook");
                    attroff(COLOR_PAIR(4));
                }

                wattron(win, COLOR_PAIR(1));
                mvwprintw(win, center+numberOfRowLevel, title, "%.2f", itr.second);
                wattroff(win, COLOR_PAIR(1));

                wattron(win, COLOR_PAIR(2));
                mvwprintw(win, center+numberOfRowLevel, title+seprator, "%.2f", itr.first);
                wattroff(win, COLOR_PAIR(2));

                numberOfRowLevel+=2;
            }

        }
        refresh();
        wrefresh(win);
    }

    void RunOrderBook()
    {
        char choice;
        while (choice != 'q')
        {
            choice = getch();
        }
    }

    ~OrderBook(){
        endwin();
    }
private:
    nMap asks;
    greaterMap bids;
    int height;
    int width;
    int yPos;
    int xPos;
    WINDOW *win;
};
