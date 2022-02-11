#include <iostream>
#include <boost/asio.hpp>

using namespace std;

void print(const boost::system::error_code& e) {
	cout << __func__ << ": Hello World!" << endl;
}

int main () {
	cout << "Creating io context."  << endl;
	boost::asio::io_context io;
	cout << "Creating timer."  << endl;
	boost::asio::steady_timer tmr(io, boost::asio::chrono::seconds(5));
	cout << "Waiting on timer::async_wait(print)."  << endl;
	tmr.async_wait(&print);
	cout << "Executing io::run()."  << endl;
	io.run();
	cout << "Timer is done."  << endl;
}


