#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <queue>

using namespace std;

#define PORT 23456

void clientHandler(int &clientFd) {
	char buffer[512] = R"({"type":"ticker","sequence":32589047814,"product_id":"ETH-USD","price":"1549.08","open_24h":"1485.35","volume_24h":"589548.02768130","low_24h":"1450.45","high_24h":"1631.32","volume_30d":"8147557.57837948","best_bid":"1548.81","best_ask":"1549.08","side":"buy","time":"2022-07-19T12:09:26.348930Z","trade_id":319738529,"last_size":"0.01803274"})";
	auto t1 = std::chrono::steady_clock::now();
	for(int i=0; i<1E6; i++) {
		send(clientFd, buffer, strlen(buffer), 0);
	}
	auto t2 = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
	std::cout << "[INFO] Time Taken By SERVER to SEND 1M messages is : " << time << " Milliseconds" << std::endl;
}

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
	// while (true)
	// {
	int client_sock = accept(sockfd, (struct sockaddr*)&addr, (socklen_t*)&len);
		// std::thread thread(clientHandler, std::ref(client_sock));
		// thread.detach();
	// }


	char buffer[512] = R"({"type":"ticker","sequence":32589047814,"product_id":"ETH-USD","price":"1549.08","open_24h":"1485.35","volume_24h":"589548.02768130","low_24h":"1450.45","high_24h":"1631.32","volume_30d":"8147557.57837948","best_bid":"1548.81","best_ask":"1549.08","side":"buy","time":"2022-07-19T12:09:26.348930Z","trade_id":319738529,"last_size":"0.01803274"})";
	char RecvBuffer[512] = "";
	std::queue<const char *>myQueue;
	auto t1 = std::chrono::steady_clock::now();
	for(int i=0; i<1E5; i++) {
		send(client_sock, buffer, strlen(buffer), 0);
		recv(client_sock, RecvBuffer, sizeof(RecvBuffer), 0);
		myQueue.push(RecvBuffer);
		// myQueue.push(std::string(RecvBuffer));
	}
	auto t2 = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
	std::cout << "[INFO] Time Taken By SERVER to SEND 1M messages is : " << time << " Milliseconds The Size Of Queue Was " << myQueue.size() << std::endl;
}


