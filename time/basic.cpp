
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace std;
using std::chrono::steady_clock;
using std::chrono::system_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::nanoseconds;
using std::chrono::seconds;

void Print(system_clock::time_point &now) {
	std::time_t now_t = system_clock::to_time_t(now);
	std::tm *now_tm = std::localtime(&now_t);
	std::cout << "Now: " << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S") 
	          << " : " << std::put_time(now_tm, "%c" ) << std::endl;
}

/** DOESN'T WORK
void Print(steady_clock::time_point &now) {
	std::time_t now_t = duration_cast<seconds>(now.time_since_epoch()).count();
	std::tm *now_tm = std::localtime(&now_t);
	std::cout << "Now: " << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S") 
	          << " : " << std::put_time(now_tm, "%c" ) << std::endl;
}
**/

int main () {
	//time_point<system_clock> t1 = system_clock::now();
	steady_clock::time_point t0 = steady_clock::now();
	system_clock::time_point t1 = system_clock::now();
	//Print(t0);
	Print(t1);

	steady_clock::time_point t2 = steady_clock::now();
	std::cout << "Time taken: " << (t2.time_since_epoch() - t0.time_since_epoch()).count() << std::endl;
	std::cout << "Time taken: " << (t2.time_since_epoch().count() - t0.time_since_epoch().count()) << std::endl;
	std::cout << "Time taken: " << duration_cast<microseconds>(t2 - t0).count() << " mics" << std::endl;
	std::cout << "Time taken: " << duration_cast<nanoseconds>(t2 - t0).count() << " nsec" << std::endl;
}

