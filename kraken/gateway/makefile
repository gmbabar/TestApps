
CC = g++

FLAGS = -std=c++17 -g -I./  -I../../include
LIBS = -lboost_system -lssl -lcrypto -lpthread

objects = balance

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)
# {"method": "subscribe","ch": "trades","params": {"symbols": ["ETHBTC", "BTCUSDT"],"limit": 1},"id": 123}
clean:
	rm -rf *.dSYM
	rm -f $(objects)

