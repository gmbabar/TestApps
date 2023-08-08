CC = g++
FLAGS = -g -std=c++11 -march=native
LIBS = -lpthread

objects = coinbase_simulator

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)
run: all
	./$(objects)
clean:
	rm -rf ./$(objects)
