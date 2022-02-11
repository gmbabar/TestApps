#include <iostream>
#include <thread>
#include <chrono>
#include <boost/bind/bind.hpp>

using namespace std;

struct TestThread {
	const int limit = 10;
	int counter = 0;
	
	void RunSimple() {
		while (++counter < limit) {
			cout << __func__ << "(): counter: " << counter << endl;
			this_thread::sleep_for( chrono::seconds(1) );
		}
	}
	static void RunStatic(int limit) {
		int counter = 0;
		while (++counter < limit) {
			cout << __func__ << "(int): counter: " << counter << endl;
			this_thread::sleep_for( chrono::seconds(1) );
		}
	}

	void CreateThread() {
		thread th ( boost::bind(&TestThread::RunSimple, this) );
		th.detach();
	}

	void CreateThreadStd() {
		thread th ( std::bind(&TestThread::RunSimple, this) );
		th.detach();
	}
};

int main () {
	int limit = 10;

	// simple - static class member
	thread th1 ( TestThread::RunStatic, limit );

	// bind - non-static class memeber
	TestThread tstObj;
	//tstObj.CreateThread();
	tstObj.CreateThreadStd();
	thread th2 ( boost::bind(&TestThread::RunSimple, tstObj) );

	th1.join();
	th2.join();

	this_thread::sleep_for( chrono::seconds(1) );
	cout << __func__ << ": Done." << endl;
}

