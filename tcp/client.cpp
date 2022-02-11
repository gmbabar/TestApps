#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>	// inet_pton, htons
//#include <netinet/in.h>  // htons

using namespace std;

#define PORT 23456

int main () {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket"); exit(EXIT_FAILURE);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
		perror("inet_pton"); exit(EXIT_FAILURE);
	}

	if (connect(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("connect"); exit(EXIT_FAILURE);
	}

	char buffer[1024];
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "Hi from client");
	send(sockfd, buffer, strlen(buffer), 0);
	cout << "Greeting sent from client" << endl;
	int size = read(sockfd, buffer, sizeof(buffer));
	cout << "received " << size << " bytes" << endl;
	cout << "Msg Rcvd: " << buffer << endl;
}


