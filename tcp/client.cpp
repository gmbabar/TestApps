#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>	// inet_pton, htons
#include <queue>
#include <chrono>
//#include <netinet/in.h>  // htons

using namespace std;

#define PORT 23456
std::queue<std::string> myQueue;

inline void parseTickerSs(const std::string &json, std::string &result) {
    std::string type;
    std::string px;
    std::string amt;

    int start = json.find("\"type\"");
    start = json.find(":", start);
    int stop = json.find("\",", start);
    type = json.substr(start+2, stop-start-2);
    start = json.find("\"price\"");
    start = json.find(":", start);
    stop = json.find("\",", start);
    px = json.substr(start+2, stop-start-2);
    start = json.find("\"last_size\"");
    start = json.find(":", start);
    stop = json.find("\"}", start);
    amt = json.substr(start+2, stop-start-2);
	result = type + "|" + px + "|" + amt;
	myQueue.push(result);
    // std::cout << "type: " << type << std::endl;
    // std::cout << "Price: " << px << std::endl;
    // std::cout << "last_size: " << amt << std::endl;
}

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
	std::string result;
	auto t1 = std::chrono::steady_clock::now();
	for (int i=0; i<1E5; i++) {
		int size = recv(sockfd, buffer, sizeof(buffer), 0);
		parseTickerSs(std::string(buffer), result);
		size = send(sockfd, result.c_str(), result.size(), 0);
	}
	auto t2 = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
	std::cout << "[INFO] Time Taken By Client To Parse And Receive 1M messages is : " << time << " Milliseconds The Size Of Queue Was " << myQueue.size() << std::endl;
}


