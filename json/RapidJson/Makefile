
CPPFLAGS = -std=c++11 -I./include

all:
	g++  $(CPPFLAGS) keyVal.cpp -o keyVal
	g++ $(CPPFLAGS) neworder.cpp -o neworder

run: all
	./keyVal
	./neworder

clean:
	rm -rf ./keyVal
	rm -rf ./neworder
