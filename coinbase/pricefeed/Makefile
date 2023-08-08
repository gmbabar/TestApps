CC = g++

FLAGS = -g -std=c++17 -I./  -I../../include  -I../include
LIBS = -lboost_system -lssl -lcrypto -lpthread

objects = gdax_pricefeed

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f $(objects)

