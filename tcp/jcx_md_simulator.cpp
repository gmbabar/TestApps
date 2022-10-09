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
    std::cout << "socket listening to port: " << PORT << std::endl;

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
        if (size == 0) break;
        cout << "read " << size << " bytes" << endl;
        std::string msg = buffer;
        std::replace( msg.begin(), msg.end(), '\x01', '|');
        cout << "Msg Rcv: " << msg << endl;
        //sprintf(buffer, "Hello from Server");
        cout << "Sending back: " << buffer << endl;
        //send( client_sock, buffer, size, 0 );
//8=FIX.4.2 9=00126 10=168 34=17037 35=X 49=JCX 50=TEST 52=20221004-15:16:28.759 56=GLXY-MD2 268=1 279=ConstBufRange[0] 55=ConstBufRange[FTM] 269=ConstBufRange[0] 270=ConstBufRange[0.1607004] 271=ConstBufRange[826.623] 278=ConstBufRange[8724]
        msg = "8=FIX.4.2|9=00126|34=17037|35=X|49=JCX|50=TEST|52=20221004-15:16:28.759|56=GLXY-MD2|268=1|279=0|55=FTM|269=0|270=0.1607004|271=8.26623|278=8724|10=168";
        std::replace( msg.begin(), msg.end(), '|', '\x01' );
        while(true) {
	    std::this_thread::sleep_for (std::chrono::seconds(1));
            send( client_sock, msg.c_str(), msg.size(), 0 );
	    std::cout << "Sent update... " << msg.size() << std::endl;
        }
    }
}


