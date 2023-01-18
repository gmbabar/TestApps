#include "client.hpp"

int main () {
	int port = 6969;
	Client client(port);
	client.start();
}


