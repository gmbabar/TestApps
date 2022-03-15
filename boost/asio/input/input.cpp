
// https://www.boost.org/doc/libs/1_78_0/doc/html/boost_asio/example/cpp03/chat/posix_chat_client.cpp

#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

namespace posix = boost::asio::posix;	// input stream / stdin


struct InputReader {
private:
    boost::asio::streambuf buffer;
    posix::stream_descriptor input_;	// stdin

public:
    explicit InputReader(boost::asio::io_service &ctx) : 
	    buffer(256), input_(ctx, ::dup(STDIN_FILENO)) {
        // Post the first recv
        std::cout << "Using boost async publish." << std::endl;
        ctx.post(boost::bind(&InputReader::inputRead, this));
    }

    void inputRead() {
        // Read a line of input entered by the user.
        std::cout << "Enter string: " << std::endl;
        boost::asio::async_read_until(input_, buffer, '\n', 
			boost::bind(&InputReader::handleReadInput, this,
				boost::placeholders::_1, boost::placeholders::_2));
				//boost::asio::placeholders::error, 
				//boost::asio::placeholders::bytes_transferred));
    }

    void handleReadInput(const boost::system::error_code& error, size_t length) {
        if (!error) {
          // Write the message to the server.
          std::ostringstream ss;
          ss << &buffer;
          std::string msg = ss.str().substr(0, length-1);	// strip-off \n
          std::cout << "Entered: '" << msg << std::endl;
        } else {
          std::cout << "Error reading input" << std::endl;
          exit(1);
        }
        inputRead();
    }
};

int main(int argc, char *argv[]) {
    boost::asio::io_service context;   // io_service is older

    InputReader reader(context);


    std::cout << "Running context" << std::endl;
    context.run();

    std::cout << "Done." << std::endl;
}


