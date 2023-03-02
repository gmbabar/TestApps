#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <netinet/in.h>

class Server {

public:
    Server(int &port, long &sendTime, std::string &msg) {
        // FIXME: member should be initialized in initializer list.
        m_msg = msg;
        m_sendTime = sendTime;
        m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
        if(m_socketFD==0) {
            perror("Socket ");
            exit(EXIT_FAILURE);
        }

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(m_socketFD, (sockaddr*)&addr, sizeof(addr)) < 0) {
		    perror("Bind ");
            exit(EXIT_FAILURE);
	    }
    }

    void start() {
        if (listen(m_socketFD, 5) < 0 ) {
		    perror("listen");
            exit(EXIT_FAILURE);
	    }
        int len = sizeof(addr);
        m_clientFD = accept(m_socketFD, (struct sockaddr*)&addr, (socklen_t*)&len);
        if(m_clientFD < 0) {
            perror("Accept ");
            exit(EXIT_FAILURE);
        }
        std::cout << "[INFO] Client Connected With Server" << std::endl;
        this->onStart();
    }
private:

    void onStart() {
        std::cout << "[INFO] Sending Messages To Client" << std::endl;
        for (int idx=0; idx<m_sendTime; idx++) {
            send(m_clientFD, m_msg.c_str(), m_msg.size(), 0);
        }
    }
    int m_socketFD;
    int m_clientFD;
    sockaddr_in addr;
    std::string m_msg;
    int m_sendTime;
};