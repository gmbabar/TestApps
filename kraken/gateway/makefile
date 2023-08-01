
CC = g++

FLAGS = -std=c++17 -g -I./  -I../../include
LIBS = -lboost_system -lssl -lcrypto -lpthread

objects = balance neworder cancelorder _neworder

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f $(objects)

