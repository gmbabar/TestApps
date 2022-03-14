all:
	g++ -g OrderBook.cpp -o OrderBook -DNCURSES_NOMACROS -lncurses -lboost_system -lpthread
run: all
	./OrderBook
clean:
	rm ./OrderBook
