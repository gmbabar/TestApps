
CC = g++

FLAGS = -g -std=c++17 -I./  -I../../include
LIBS = -lboost_system -lssl -lcrypto -lpthread 

objects = coinbase_rest

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f $(objects)

