
CC = g++

FLAGS = -g -std=c++17 -I./  -I../../include
LIBS = -lboost_system -lssl -lcrypto -lpthread

objects = coinbase_get_products coinbase_send_order coinbase_get_balance coinbase_get_orders coinbase_get_order

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f $(objects)

