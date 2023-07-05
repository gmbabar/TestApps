#include <iostream>
#include <string>

using namespace std;

// special template
void print() {
	std::cout << __func__ << "(): " << std::endl;
}

// variadic template
template <typename F, typename... R>
void print(const F& first, const R& ... rest) {	// Note: & comes before ... in case of reference
	std::cout << __func__ << "(...): " << first << " "; 
	print(rest...);	// recursive call using pack expansion
}

int main () {
	print();
	print(10);
	print(10, "hello", 24.5);
}

