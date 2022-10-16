
# Mac OS only: /usr/local/opt/openssl/include
FLAGS = -g -std=c++17 -I./ -I/usr/local/opt/openssl/include -I./include -I../include
LIBS = -L/usr/local/lib -lssl -lboost_system -lcrypto -lpthread

all:
	g++ $(FLAGS) pricefeed.cpp -o pricefeed $(LIBS)

clean:
	rm -rf pricefeed pricefeed.dSYM


