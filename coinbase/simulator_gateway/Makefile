CC = g++
FLAGS = -g -std=c++11 -march=native
LIBS = -lpthread -lboost_system -lcrypto -lssl

objects = coinbase_simulator

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)
run: all
	./$(objects)
clean:
	rm -rf ./$(objects)
