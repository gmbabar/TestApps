#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <iostream>

using namespace std;

#define PORT 23456

int main () {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == 0) {
		perror("socket error"); exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt"); exit(EXIT_FAILURE);
	}

	// set socket address.
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons( PORT );

	// bind socket
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind error"); exit(EXIT_FAILURE);
	}

	// listen on socket
	if (listen(sockfd, 3) < 0 ) {
		perror("listen error"); exit(EXIT_FAILURE);
	}

	// accept socket connection
	int len = sizeof(addr);
	int client_sock = accept(sockfd, (struct sockaddr*)&addr, (socklen_t*)&len);
        if (client_sock < 0) {
		perror("accept"); exit(EXIT_FAILURE);
	}

	while (true) {
		// read data
		char buffer[1024];
		int size = read(client_sock, buffer, sizeof(buffer));
		cout << "read " << size << " bytes" << endl;
		cout << "Msg Rcv: " << buffer << endl;
		sprintf(buffer, "Hello from Server");
		send( client_sock, buffer, strlen(buffer), 0 );
	}
}


