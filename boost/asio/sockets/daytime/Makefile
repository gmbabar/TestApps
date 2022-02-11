
all: sync_daytime_client.cpp sync_daytime_server.cpp 
	g++ -std=c++11 -o sync_daytime_client sync_daytime_client.cpp -lboost_system -lpthread
	g++ -std=c++11 -o sync_daytime_server sync_daytime_server.cpp -lboost_system -lpthread

one:
	g++ -o async_daytime_server async_daytime_server.cpp -lboost_system -lboost_thread -lpthread

clean:
	rm -f sync_daytime_client sync_daytime_server
	rm -f `ls | egrep -v "Makefile|*.cpp" | xargs`


