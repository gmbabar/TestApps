# Mac OS only: /usr/local/opt/openssl/include
FLAGS = -g -std=c++17 -I./ -I/usr/local/opt/openssl/include
LIBS = -L/usr/local/lib -lssl -lboost_system -lcrypto -lpthread

all:
	mkdir ./bin
	g++ $(FLAGS) fix_api.cpp -o ./bin/fix_api $(LIBS)

clean:
	rm -rf fix_api fix_api.dSYM


