#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <netinet/in.h>
#include <queue>
#include "parser.hpp"

class Client {
public:
    Client(int &port) {
        m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
        if(m_socketFD==0) {
            perror("Socket ");
            exit(EXIT_FAILURE);
        }

        m_serverAddr.sin_family = AF_INET;
        m_serverAddr.sin_addr.s_addr = INADDR_ANY;
        m_serverAddr.sin_port = htons(port);
        if (inet_pton(AF_INET, "127.0.0.1",
            &m_serverAddr.sin_addr) <= 0) {
            perror("inet_pton");
            exit(EXIT_FAILURE);
	    }
    }

    void start() {
        if (connect(m_socketFD, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr)) < 0) {
		    perror("connect");
            exit(EXIT_FAILURE);
	    }
        this->onStart();
    }
private:

    void onStart() {
        auto initialTime = std::chrono::steady_clock::now();
        while (recv(m_socketFD, m_buffer, sizeof(m_buffer), 0) > 0)
        {
            parseMsg(std::string(m_buffer), m_result);
            m_parsedQueue.push(m_result);
        }
        auto finalTime = std::chrono::steady_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(finalTime-initialTime);
        std::cout << "[INFO] Total Time To Parse Messages Is " << totalTime.count()
                << "MICS, The Queue Size Is " << m_parsedQueue.size() << std::endl;
    }
    int m_socketFD;
    sockaddr_in m_serverAddr;
    char m_buffer[512];
    std::string m_result;
    std::queue<std::string> m_parsedQueue;
};