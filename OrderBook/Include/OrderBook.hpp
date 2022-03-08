#include <ncurses.h>
#include <map>

typedef std::map<float, float, std::greater<float>> greaterMap;
typedef std::map<float, float> nMap;

class OrderBook{
public:
    explicit OrderBook(int height, int width, int yPos, int xPos) :  height(height), width(width), yPos(yPos), xPos(xPos) {
        // initscr();
        Border();
    }

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

        // refresh();
        // wrefresh(win);
        // while (c != 'x')
        // {
        //     c = getch();
        // }
    }

    void AddData(float price, float quantity, bool ask)
    {

        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        char c;
        int seprator = width/3;
        int title = seprator/2;
        int numberOfRowLevel = 1;
        // mvprintw(1, 2, "%d", numberOfRowLevel);
        if(ask)
        {
            if(center-numberOfRowLevel < 4)
            {
                attron(COLOR_PAIR(4));
                mvprintw(height+5, seprator+title, "\t[Warning] Columns Filled The Text Will Not Appear After Specific Height");
                mvprintw(height+6, seprator+title, "\t[Hint] Increase Heigh Of OrderBook");
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
            mvprintw(1, 2, "Else Block");
            bids.emplace(price, quantity);
            for(auto itr: bids)
            {
                if(numberOfRowLevel== center-3)
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(height+5, seprator+title, "\t[Warning] Columns Filled The Text Will Not Appear After Specific Height");
                    mvprintw(height+6, seprator+title, "\t[Hint] Increase Heigh Of OrderBook");
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

    void Run()
    {
        char c;
        while (c != 'q')
        {
            c = getch();
        }
    }

    ~OrderBook(){
        // endwin();
    }
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
