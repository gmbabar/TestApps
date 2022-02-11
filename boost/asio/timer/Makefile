
all: timer1.cpp timer2.cpp
	g++ -o timer1 timer1.cpp -lboost_system -lboost_thread -lpthread
	g++ -o timer2 timer2.cpp -lboost_system -lboost_thread -lpthread
	g++ -o timer3 timer3.cpp -lboost_system -lboost_thread -lpthread
	g++ -o timer_cls timer_cls.cpp -lboost_system -lboost_thread -lpthread

one:
	g++ -o timer_cls timer_cls.cpp -lboost_system -lboost_thread -lpthread

clean:
	rm -f timer1 timer2 timer3 timer_cls

