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
typedef boost::asio::ip::udp udpSocket;

namespace asio = boost::asio;

struct Data{
    float price;
    float quantity;
    int isAsk;
};

class OrderBook{
public:
    OrderBook(context &ctx) : ctx(ctx), uSocket(ctx)
    {
        uSocket.open(udpSocket::v4());
        uSocket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        uSocket.bind(udpSocket::endpoint(boost::asio::ip::make_address("127.0.0.1"), 1234));
        initscr();
        noecho();
        getmaxyx(stdscr, scrMaxY, scrMaxX);
        win_ = newwin(scrMaxY/1.25, scrMaxX/1.2, scrMaxY/10, scrMaxX/10);
        getmaxyx(win_, winMaxY, winMaxX);
        this->Border();
    }

    void Border()
    {
        float addition = (winMaxY-6);
        addition = addition/20;
        
        int seprator = winMaxX/3;
        int center = winMaxY/2;

        start_color();

        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        box(win_, 0,0);
        
        for(float i = addition*3-1; i < winMaxY; i+=addition)
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

    void AddData(float price, float quantity, bool ask)
    {
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);

        float addition = (winMaxY-5);
        addition = addition/20;

        int center = (winMaxY/2)+2;
        int seprator = winMaxX/3;
        int title = seprator/2;
        long int numberOfRowLevel = 1;
        if(ask)
        {
            asks[price] += quantity;
            for(auto itr: asks)
            {
                if(center-numberOfRowLevel < (addition-0.5)*3)
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
        }
        else
        {
            bids[price] += quantity;
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

        }
        refresh();
        wrefresh(win_);
    }

    void RecieveFromSocket()
    {
        ctx.post(boost::bind(&OrderBook::SetupRecieve, this));
    }

    ~OrderBook(){
        endwin();
    }
private:
    void SetupRecieve()
    {
        uSocket.async_receive_from(boost::asio::buffer(buffer), clientEP, boost::bind(&OrderBook::ReadHandle, this,
        boost::placeholders::_1, boost::placeholders::_2));
    }

    void ReadHandle(errorCode &ec, size_t bytesRead)
    {
        sscanf(buffer, "%f, %f, %d", &d1.price, &d1.quantity, &d1.isAsk);
        AddData(d1.price, d1.quantity, d1.isAsk);
        SetupRecieve();
    }

    nMap asks;
    greaterMap bids;
    int scrMaxX, scrMaxY, winMaxX, winMaxY;
    WINDOW *win_;
    context &ctx;
    Data d1;
    char buffer[128];
    udpSocket::endpoint clientEP;
    udpSocket::socket uSocket;
};