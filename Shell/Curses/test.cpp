
#include <stdio.h>
#include <curses.h>
#include <term.h>

#define CONTROL(x)  ((x) & 0x1F)

int main(void)
{
    FILE *fp = fopen("x", "w");
    if (fp == 0)
        return(-1);
    SCREEN *s = newterm(NULL, stdin, stdout);
    if (s == 0)
        return(-1);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int ch;
    while ((ch = getch()) != EOF && ch != CONTROL('d'))
        fprintf(fp, "%d\n", ch);

    endwin();

    return 0;
}


