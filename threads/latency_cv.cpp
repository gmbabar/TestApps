#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>

using namespace std;

bool exit_ = false;
condition_variable cv_;
mutex mtx_;

const long MaxCount = 10000;

void Producer () {
	long long start_time = chrono::duration_cast<chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	cout << "Producer signalling" << endl;
	for (unsigned idx = 0; idx<MaxCount; ++idx) {
		cv_.notify_one();
	}
	exit_ = true;
	cv_.notify_all();
	long long end_time = chrono::duration_cast<chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	cout << "Total time: " << end_time - start_time << ", Avg: " << (end_time - start_time) / MaxCount << endl;
}

void Consumer() {
	cout << "Consumer started." << endl;
	unique_lock<mutex> lck(mtx_);
	while(!exit_) {
		cv_.wait(lck);
	}
	cout << "Consumer done." << endl;
}


int main () {
	cout << "Main: Threading." << endl;
	thread prdThread( Producer );
	for( int idx=0; idx<50; ++idx ) {
	     thread cnsThread( Consumer );
	     cnsThread.detach();
	}

	cout << "Main: Joining." << endl;
	prdThread.join();

	cout << "Main: Done." << endl;
}


