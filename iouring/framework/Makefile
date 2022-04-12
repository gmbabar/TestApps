
all:
	g++ UdpSocket.cpp -o UdpSocket -std=c++11 -luring
	g++ TcpSocket.cpp -o TcpSocket -std=c++11 -luring

one:
	g++ IouringPost.cpp -o IouringPost -std=c++14 -lpthread -luring

clean:
	rm -f UdpSocket TcpSocket IouringPost
