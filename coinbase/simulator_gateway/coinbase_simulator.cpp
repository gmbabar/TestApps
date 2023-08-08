#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace std;

#define PORT 65432


struct Session {
    Session() {

        m_sockFd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_sockFd == 0) {
            perror("socket error"); exit(EXIT_FAILURE);
        }

        if (setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &m_opt, sizeof(m_opt))) {
            perror("setsockopt"); exit(EXIT_FAILURE);
        }

        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = INADDR_ANY;
        m_addr.sin_port = htons(PORT);
        this->SetupSocket();
    }

    void run() {
        int len = sizeof(m_addr);
        m_clientFd = accept(m_sockFd, (struct sockaddr*)&m_addr, (socklen_t*)&len);
        if (m_clientFd < 0) {
            perror("accept"); exit(EXIT_FAILURE);
        }


        while (true) {
        // read data
            char buffer[1024];
            int size = read(m_clientFd, buffer, sizeof(buffer));
            if (size == 0) break;
            cout << "read " << size << " bytes" << endl;
            std::string msg = buffer;
            std::replace(msg.begin(), msg.end(), '\x01', '|');
            cout << "Msg Rcv: " << msg << endl;
            //sprintf(buffer, "Hello from Server");


            const auto type = this->parseResponse(msg);
            if(type == 'A') {
                msg = "\n\n8=FIX.4.2|9=172|35=A|34=1|49=Coinbase|52=20230808-14:46:30.660|56=c636bc5e6a36f1089f90e1c05ccc4d18|96=vqu3SkWPCuGkH2vo+TNtaCrv4hclSu9KBJdZd9UlJ+M=|98=0|108=30|141=Y|554=ncz86pr00bh|8013=Y|10=013|\n\n";
                std::replace(msg.begin(), msg.end(), '|', '\x01');
                int ec = send(m_clientFd, msg.c_str(), msg.size(), 0);
                if(ec < 0) {
                    perror("send"); exit(EXIT_FAILURE);
                }
                cout << "Sending back: " << msg << endl;
            }
            else if (type == '0') {
                msg = "\n\n8=FIX.4.2|9=58|35=0|49=Coinbase|56=c636bc5e6a36f1089f90e1c05ccc4d18|34=4|52=20190605-12:19:52.060|10=165\n\n";
                std::replace(msg.begin(), msg.end(), '|', '\x01');
                int ec = send(m_clientFd, msg.c_str(), msg.size(), 0);
                if(ec < 0) {
                    perror("send"); exit(EXIT_FAILURE);
                }
                cout << "Sending back: " << msg << endl;
            }
            memset(buffer,0,sizeof(buffer));
            if(++m_sentCount == 15) {
                break;
            }
        }
    }

private:

    void SetupSocket() {
        if (bind(m_sockFd, (struct sockaddr*)&m_addr, sizeof(m_addr)) < 0) {
            perror("bind error"); exit(EXIT_FAILURE);
        }

        if (listen(m_sockFd, 5) < 0) {
            perror("listen error"); exit(EXIT_FAILURE);
        }
        std::cout << "socket listening to port: " << PORT << std::endl;
    }

    void closeSocket() {
        close(m_clientFd);
        close(m_sockFd);
        std::cout << "[INFO] Sessions closed.\n";
        this->run();
    }

    char parseResponse(const std::string &msg) {
        auto key = msg.find("|35");
        if(key == std::string::npos) {
            key = msg.find('\x01'+"35");
            if (key == std::string::npos) {
                return '\0';
            }
        }
        const auto init = msg.find('=', key+1);
        if(init == std::string::npos) {
            return '\0';
        }

        const auto result = msg[init+1];
        std::cout << "MSG_TYPE: " << result << std::endl;
        return result;
    }


    int m_sockFd;
    int m_clientFd;
    int m_sentCount = 0;
    const int m_opt = 1;
    sockaddr_in m_addr;
};


int main () {
    Session server;
    server.run();
}


// 8=FIX.4.2|9=172|35=A|34=1|49=c636bc5e6a36f1089f90e1c05ccc4d18|56=Coinbase|98=0|108=30|141=Y|52=20230808-14:46:30.613|554=ncz86pr00bh|96=vqu3SkWPCuGkH2vo+TNtaCrv4hclSu9KBJdZd9UlJ+M=|8013=Y|10=011|
// 8=FIX.4.2|9=172|35=A|34=1|49=Coinbase|52=20230808-14:46:30.660|56=c636bc5e6a36f1089f90e1c05ccc4d18|96=vqu3SkWPCuGkH2vo+TNtaCrv4hclSu9KBJdZd9UlJ+M=|98=0|108=30|141=Y|554=ncz86pr00bh|8013=Y|10=013|


// 8=FIX.4.2|9=58|35=0|49=c636bc5e6a36f1089f90e1c05ccc4d18|56=Coinbase|34=4|52=20190605-12:19:52.060|10=165|