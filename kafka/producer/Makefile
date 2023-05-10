
CC = g++

FLAGS = -g -std=c++11 -I./  -I/home/users/gbabar/libkafka/include -L/home/users/gbabar/libkafka/lib

LIBS = -lboost_system -lpthread -lrdkafka++

objects = producer

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f $(objects)

