
CC = g++

FLAGS = -std=c++17 -g -I./  -I../../include -Wall
LIBS = -lboost_system -lssl -lcrypto -lpthread

objects =  rest_parser ws_parser


all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f $(objects)

