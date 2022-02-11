#include <iostream>
#include <boost/asio.hpp>

using namespace std;


int main () {
	cout << "Creating io context."  << endl;
	boost::asio::io_context io;
	cout << "Creating timer."  << endl;
	boost::asio::steady_timer tmr(io, boost::asio::chrono::seconds(5));
	cout << "Waiting on timer."  << endl;
	tmr.wait();
	cout << "Timer is done."  << endl;
}


