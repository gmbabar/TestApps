#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>

void thread_func() {
	for ( int i = 0; i < 5; ++i ) {
		boost::this_thread::sleep_for(boost::chrono::seconds(1));
		std::cout << "i: " << i << std::endl;
	}
	std::cout << __func__ << "Done." << std::endl;
}

int main() {
	boost::thread thrd(thread_func);
	thrd.join();
}

