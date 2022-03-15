
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <ncurses.h>

#define CTRL_R    18
#define CTRL_C    3
#define TAB       9
#define NEWLINE   10
#define RETURN    13
#define ESCAPE    27
#define BACKSPACE 127
#define UP        72
#define LEFT      75
#define RIGHT     77
#define DOWN      80

struct Program {
    Program() {
        initscr();
        ESCDELAY = 0;
        timeout(0);
        cbreak(); 

        noecho();
        keypad(stdscr, TRUE); // receive special keys

        clock   = newwin(2, 40, 0, 0);
        monitor = newwin(10, 40, 2, 0);

        syncok(clock, true);    // automatic updating
        syncok(monitor, true);

        scrollok(monitor, true); // scroll the input monitor window
    }
    ~Program() {
        delwin(monitor);
        delwin(clock);
        endwin();
    }

    void on_clock() {
        wclear(clock);

        char buf[32];
        time_t t = time(NULL);
        if (auto tmp = localtime(&t)) {
            if (strftime(buf, sizeof(buf), "%T", tmp) == 0) {
                strncpy(buf, "[error formatting time]", sizeof(buf));
            }
        } else {
            strncpy(buf, "[error getting time]", sizeof(buf));
        }

        wprintw(clock, "Async: %s", buf);
        wrefresh(clock);
    }

    void on_input() {
        for (auto ch = getch(); ch != ERR; ch = getch()) {
            wprintw(monitor, "received key %d ('%c')\n", ch, ch);
        }
        wrefresh(monitor);
    }

    WINDOW *monitor = nullptr;
    WINDOW *clock = nullptr;
};

int main() {
    Program program;

    namespace ba = boost::asio;
    using boost::system::error_code;
    using namespace std::literals;

    ba::io_service io;
    std::function<void(error_code)> input_loop, clock_loop;

    // Reading input when ready on stdin
    ba::posix::stream_descriptor d(io, 0);
    input_loop = [&](error_code ec) {
        if (!ec) {
            program.on_input();
            d.async_wait(ba::posix::descriptor::wait_type::wait_read, input_loop);
        }
    };

    // For fun, let's also update the time
    ba::high_resolution_timer tim(io);
    clock_loop = [&](error_code ec) {
        if (!ec) {
            program.on_clock();
            tim.expires_from_now(100ms);
            tim.async_wait(clock_loop);
        }
    };

    input_loop(error_code{});
    clock_loop(error_code{});
    io.run_for(10s);
}

