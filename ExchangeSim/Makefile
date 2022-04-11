CC = g++
FLAGS = -g -std=c++11
LIBS = -lpthread

objects = thread-base

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)
run: all
	./$(objects)
clean:
	rm -rf ./$(objects)
