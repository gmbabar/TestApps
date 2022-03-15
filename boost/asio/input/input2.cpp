
// https://www.boost.org/doc/libs/1_78_0/doc/html/boost_asio/example/cpp03/chat/posix_chat_client.cpp

#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <ncurses.h>

namespace posix = boost::asio::posix;	// input stream / stdin


struct InputReader {
private:
    boost::asio::streambuf buffer;
    posix::stream_descriptor input_;	// stdin
    WINDOW *_new = nullptr;

public:
    explicit InputReader(boost::asio::io_service &ctx) : 
	    buffer(256), input_(ctx, ::dup(STDIN_FILENO)) {
        // Post the first recv
        std::cout << "Using boost async publish." << std::endl;
        ctx.post(boost::bind(&InputReader::inputRead, this));
	// init ncurses:
	screenInit();
    }

    ~InputReader() {
	    delwin(_new);
	    endwin();
    }

    void screenInit() {
        initscr();

        timeout(0);
        cbreak();
        noecho();
        keypad(stdscr, TRUE); // receive special keys

        int i = 2, height, width;

        getmaxyx(stdscr, height, width);
        _new = newwin(height - 2, width - 2, 1, 1);

	syncok(_new, true);
        scrollok(_new,TRUE);
    }

    void inputRead() {
        // Read a line of input entered by the user.
        std::cout << "Enter Input: " << std::endl;
	input_.async_wait(posix::descriptor::wait_type::wait_read, 
			  boost::bind(&InputReader::handleInput, this, boost::placeholders::_1));
    }

    void handleInput(const boost::system::error_code& error) {
        for (auto ch = getch(); ch != ERR; ch = getch()) {
            wprintw(_new, "received key %d ('%c')\n", ch, ch);
        }
        wrefresh(_new);
	inputRead();
    }
};

int main(int argc, char *argv[]) {
    boost::asio::io_context context;   

    InputReader reader(context);

    std::cout << "Running context" << std::endl;
    context.run();

    std::cout << "Done." << std::endl;
}


