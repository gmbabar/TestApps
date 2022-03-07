#include <ncurses.h>
#include <map>


class OrderBook{
public:
    explicit OrderBook(int height, int width, int yPos, int xPos) :  height_(height), width_(width), y_pos_(yPos), x_pos_(xPos) {}

    void Border()
    {
        keypad(win, true);
        start_color();

        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        char c;
        int seprator = width_/3;
        box(win, 0,0);
        for(int i = 3; i < height_; i++)
        {
            for(int j = 1; j < width_-1; j++)
            {
                mvwprintw(win, i, j, "_");
            }
            mvwprintw(win, i, seprator, "|");
            mvwprintw(win, i, seprator+seprator, "|");
        }

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

        refresh();
        wrefresh(win);
        while (c != 'x')
        {
            c = getch();
        }
        
        getch();
    }

    void AddData(float price, float quantity, bool is_ask)
    {
        int seprator = width_/3;
        int title = seprator/2;
        if(is_ask)
        {
            int number_of_row_level = 0;
            ask.emplace(price, quantity);
            for(auto itr: ask)
            {
                mvwprintw(win, center_-number_of_row_level, title+seprator, "%f", itr.first);
                mvwprintw(win, center_-number_of_row_level, title+2*seprator, "%f", itr.second);
                number_of_row_level+=2;
            }
        }
        else
        {
            int number_of_row_level = 0;
            bid.emplace(price, quantity);
            for(auto itr: bid)
            {
                mvwprintw(win, center_+number_of_row_level, title+seprator, "%f", itr.first);
                mvwprintw(win, center_+number_of_row_level, title, "%f", itr.second);
                number_of_row_level+=2;
            }
        }
        refresh();
        wrefresh(win);
    }

    ~OrderBook(){}
private:
    std::map<float, float> ask, bid;
    int height_;
    int width_;
    int center_ = height_/2;
    int y_pos_;
    int x_pos_;
    WINDOW *win = newwin(height_, width_, y_pos_, x_pos_);
};
