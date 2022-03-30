
CC = g++
FLAGS = -g -std=c++11
LIBS = -lboost_system -lpthread -lcrypto -lssl

objects = websocket_client websocket_server websocket_client_ssl websocket_server_ssl

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

# Inactive.
ServerSimulator: ServerSimulator.cpp
	g++ $(FLAGS) -o $@  $< $(LIBS)

clean:
	rm -f `ls | egrep -v "Makefile|*.cpp|*.hpp|*.h" | xargs`

