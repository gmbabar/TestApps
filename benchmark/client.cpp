#include "client.hpp"

int main (int argc, char **argv) {
	if (argc != 2) {
        std::cerr << "Arguement Port Missing" << std::endl;
        exit(EXIT_FAILURE);
    }
	int port = std::stoi(argv[1]);
	Client client(port);
	client.start();
}


