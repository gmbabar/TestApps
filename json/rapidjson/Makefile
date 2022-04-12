
CC = g++
FLAGS = -g -std=c++11 -I./include
LIBS = -lboost_system -lpthread -lcrypto -lssl

objects = keyVal neworder subscribe

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

# Inactive.
ServerSimulator: ServerSimulator.cpp
	g++ $(FLAGS) -o $@  $< $(LIBS)

clean:
	rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|include" | xargs`
	rm -rf *.dSYM


