all:
	g++ -g OrderBook.cpp -o OrderBook -DNCURSES_NOMACROS -lncurses -lboost_system -lpthread
	g++ -g client.cpp -o cli -lboost_system -lpthread
run: all
	./OrderBook
clean:
	rm -rf ./OrderBook
	rm -rf ./cli
