
timer: timer.o
	g++ -o timer timer.o -lboost_system -lboost_thread -lpthread

timer.o: timer.cpp
	g++ -c timer.cpp

clean:
	rm -f *.o timer

