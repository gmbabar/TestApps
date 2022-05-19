
CC = g++
FLAGS = -g -std=c++11 -I../include
LIBS = -lboost_system -lpthread -lcrypto -lssl

objects = subscribe unsubscribe SymbolStatus ExchangeStatus L1Update L2Update L3Update TradeUpdate ExchangeCapabilities L1Snapshot L2Snapshot L3Snapshot

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f `find . -maxdepth 1 -type f | egrep -v "Makefile|\.cpp|\.hpp|\.h|include|git" | xargs`


