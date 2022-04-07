
CC = g++
FLAGS = -g -std=c++11
LIBS = -lboost_system -lpthread -lcrypto -lssl

objects = auth open trade feed error cancel subscribe neworder

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -f `ls | egrep -v "Makefile|*.cpp|*.hpp|*.h" | xargs`

