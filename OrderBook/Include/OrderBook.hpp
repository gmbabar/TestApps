#include <ncurses.h>
#include <map>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

typedef std::map<float, float, std::greater<float>> greaterMap;
typedef std::map<float, float> nMap;
typedef boost::asio::io_context context;
typedef boost::asio::posix::stream_descriptor descriptor;
typedef boost::asio::streambuf buffer;
typedef const boost::system::error_code errorCode;

namespace asio = boost::asio;

class OrderBook{
public:
    OrderBook(int heightOfBook, int widthOfBook, int yPosition, int xPosition, context &ctx) :  
	    height(heightOfBook), width(widthOfBook), yPos(yPosition), xPos(xPosition), 
	    ctx(ctx), input_(ctx, ::dup(STDIN_FILENO)) 
    {
        initscr();
        noecho();
        win_ = newwin(height, width, yPos, xPos);
        Border();
    }

    void Border()
    {
        int seprator = width/3;
        int center = height/2;
        int maxX = getmaxx(win_)-2;
        int maxY = getmaxy(win_);

        start_color();

        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        box(win_, 0,0);

        for (int i = 4; i < height; i+=2)
        {
            mvwhline(win_, i, 1, ACS_HLINE, maxX);
        }
        
        mvwvline(win_, 1, seprator, ACS_VLINE, maxY);
        mvwvline(win_, 1, seprator*2, ACS_VLINE, maxY);

        int title = seprator/2;
        wattron(win_, COLOR_PAIR(1));
        mvwprintw(win_, 2, title, "\"BID\"");
        wattroff(win_, COLOR_PAIR(1));

        wattron(win_, COLOR_PAIR(2));
        mvwprintw(win_, 2, title+seprator, "\"Price\"");
        wattroff(win_, COLOR_PAIR(2));
        
        wattron(win_, COLOR_PAIR(3));
        mvwprintw(win_, 2, title+2*seprator, "\"ASK\"");        
        wattroff(win_, COLOR_PAIR(3));

        attron(COLOR_PAIR(4));
        mvprintw(height+3, title+seprator, "\tPress \'q And return\' To Exit");
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
                wattron(win_, COLOR_PAIR(3));
                mvwprintw(win_, center-numberOfRowLevel, title+2*seprator, "%.2f", itr.second);
                wattroff(win_, COLOR_PAIR(3));
                
                wattron(win_, COLOR_PAIR(2));
                mvwprintw(win_, center-numberOfRowLevel, title+seprator, "%.2f", itr.first);
                wattroff(win_, COLOR_PAIR(2));
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

                wattron(win_, COLOR_PAIR(1));
                mvwprintw(win_, center+numberOfRowLevel, title, "%.2f", itr.second);
                wattroff(win_, COLOR_PAIR(1));

                wattron(win_, COLOR_PAIR(2));
                mvwprintw(win_, center+numberOfRowLevel, title+seprator, "%.2f", itr.first);
                wattroff(win_, COLOR_PAIR(2));

                numberOfRowLevel+=2;
            }

        }
        refresh();
        wrefresh(win_);
    }

    void RunOrderBook()
    {
        ctx.post(boost::bind(&OrderBook::ReadInput, this));
    }

    ~OrderBook(){
        endwin();
    }
private:
    void ReadHandler(const errorCode &ec){
        for (auto ch = getch(); ch != ERR; ch = getch()) {
            mvwprintw(win_, 41, 85, "received key %d ('%c')\n", ch, ch);
        }
        wrefresh(win_);
        ReadInput();
    }

    void ReadInput(){        
        // Read a line of input entered by the user.
        mvwprintw(win_, 41, 85, "Enter Input: " );
        wrefresh(win_);
        input_.async_wait(boost::asio::posix::descriptor::wait_type::wait_read,
                          boost::bind(&OrderBook::ReadHandler, this, boost::placeholders::_1));
    }

    nMap asks;
    greaterMap bids;
    int height;
    int width;
    int yPos;
    int xPos;
    WINDOW *win_;
    context &ctx;
    descriptor input_;
    buffer buff;
};


