
#include <ncurses.h>

int main(void)
{
    int i = 2, height, width;
    WINDOW *_new;

    initscr();
    getmaxyx(stdscr, height, width);
    _new = newwin(height - 2, width - 2, 1, 1);

    scrollok(_new,TRUE);

    while(i < 120)
    {
        wprintw(_new, "%d - lots and lots of lines flowing down the terminal\n", i);
        ++i;
        wrefresh(_new);
    }

    int ch = getch();
    wprintw(_new, "%d", ch );
    wrefresh(_new);

    ch = getch();
    wprintw(_new, "%d", ch );
    wrefresh(_new);
    ch = getch();
    wprintw(_new, "%d", ch );
    wrefresh(_new);
    getch();
    getch();
    getch();
    endwin();
    return 0;
}

